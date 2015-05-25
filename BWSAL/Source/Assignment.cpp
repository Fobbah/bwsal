#include <vector>
#include <map>
#include <BWSAL/RectangleArray.h>
namespace BWSAL
{
  std::map< int, int > assignments;
  void makeAssignments( RectangleArray< double > &cost );

  // Hungarian Algorithm
  // http://en.wikipedia.org/wiki/Hungarian_algorithm
  // Input: Matrix where Cij is the cost of assigning row i to column j
  // Output: Optimal assignments from rows to columns.

  std::map< int, int >& computeAssignments( RectangleArray< double > &cost )
  {
    assignments.clear();

    if ( cost.getWidth() == 0 || cost.getHeight() == 0 )
    {
      // Nothing to assign so return an empty map
      return assignments;
    }

    // Initialize mark and lined rows and columns
    std::vector< bool > markedRows( cost.getHeight(), false );
    std::vector< bool > markedColumns( cost.getWidth(), false );
    std::vector< bool > linedRows( cost.getHeight(), false );
    std::vector< bool > linedColumns( cost.getWidth(), false );

    // Repeat the procedure ( steps 1–4 ) until an assignment is possible
    while ( true )
    {

      // Step 1: From each row subtract off the row minimum
      for ( int r = 0; r < (int)cost.getHeight(); r++ )
      {
        double minCost = cost[r][0];
        for ( int c = 1; c < (int)cost.getWidth(); c++ )
        {
          if ( cost[r][c] < minCost )
          {
            minCost = cost[r][c];
          }
        }
        // minCost is the minimum cost in row r.
        // Subtract minCost from each element in this row
        for ( int c = 0; c < (int)cost.getWidth(); c++ )
        {
          cost[r][c] -= minCost;
        }
      }

      // Step 2: From each column subtract off the column minimum
      for ( int c = 0; c < (int)cost.getWidth(); c++ )
      {
        double minCost = cost[0][c];
        for ( int r = 1; r < (int)cost.getHeight(); r++ )
        {
          if ( cost[r][c] < minCost )
          {
            minCost = cost[r][c];
          }
        }
        // minCost is the minimum cost in column c.
        // Subtract minCost from each element in this column
        for ( int r = 0; r < (int)cost.getHeight(); r++ )
        {
          cost[r][c] -= minCost;
        }
      }

      // Step 3: Use as few lines as possible to cover all zeros in the matrix
      // Assign as many task as possible
      makeAssignments( cost );

      // If we have assigned everything, return it!
      if ( assignments.size() == cost.getHeight() )
      {
        return assignments;
      }
      // Otherwise:

      // Reset all rows and columns to false for marked and lined
      // And then mark all rows having no assignments
      for ( int r = 0; r < (int)cost.getHeight(); r++ )
      {
        markedRows[r] = ( assignments.find( r ) == assignments.end() );
        linedRows[r] = false;
      }
      for ( int c = 0; c < (int)cost.getWidth(); c++ )
      {
        markedColumns[c] = false;
        linedColumns[c] = false;
      }

      // Repeat until no new markings are made
      bool done = false;
      while ( done == false ) // At most r + c iterations
      {
        done = true; // Assume we are done unless we make a new marking

        // Mark all columns that have a zero in one or more of the marked rows
        for ( int r = 0; r < (int)cost.getHeight(); r++ )
        {
          if ( markedRows[r] )
          {
            for ( int c = 0; c < (int)cost.getWidth(); c++ )
            {
              if ( cost[r][c] == 0 )
              {
                // We found a column with a zero in this marked row
                if ( !markedColumns[c] )
                {
                  // Make a new marking
                  done = false;
                  markedColumns[c] = true;
                }
              }
            }
          }
        }

        // Mark all rows that have assignments in one of more of the marked columns
        for ( int c = 0; c < (int)cost.getWidth(); c++ )
        {
          if ( markedColumns[c] )
          {
            for ( int r = 0; r < (int)cost.getHeight(); r++ )
            {
              if ( assignments.find( r ) != assignments.end() && assignments.find( r )->second == c )
              {
                // This row has an assignment in this column
                if ( !markedRows[r] )
                {
                  // Make a new marking
                  done = false;
                  markedRows[r] = true;
                }
              }
            }
          }
        }
      }

      // Draw lines through all unmarked rows
      int k = 0;
      for ( int r = 0; r < (int)cost.getHeight(); r++ )
      {
        linedRows[r] = !markedRows[r];
        if ( linedRows[r] )
        {
          k++;
        }
      }
      // Draw lines through all marked columns
      for ( int c = 0; c < (int)cost.getWidth(); c++ )
      {
        linedColumns[c] = markedColumns[c];
        if ( linedColumns[c] )
        {
          k++;
        }
      }

      // If the number of marked lines and unmarked rows
      // is equal to the number of columns, we are done
      if ( k == cost.getWidth() )
      {
        break;
      }

      // Set m to the minimum uncovered number
      double m;
      bool setM = false;
      for ( int r = 0; r < (int)cost.getHeight(); r++ )
      {
        for ( int c = 0; c < (int)cost.getWidth(); c++ )
        {
          if ( !linedRows[r] && !linedColumns[c] )
          {
            // This element has no row or column line on it
            // So it is uncovered
            if ( !setM || cost[r][c] < m )
            {
              m = cost[r][c];
              setM = true;
            }
          }
        }
      }

      // Subtract m from every uncovered number
      // And add m to every number covered with 2 lines
      for ( int r = 0; r < (int)cost.getHeight(); r++ )
      {
        for ( int c = 0; c < (int)cost.getWidth(); c++ )
        {
          if ( !linedColumns[c] && !linedRows[r] )
          {
            // This element has no row or column line on it
            // So it is uncovered
            cost[r][c] -= m;
          }
          else if ( linedColumns[c] && linedRows[r] )
          {
            cost[r][c] += m;
          }
          // In the case of covered by just 1 line, do nothing
        }
      }
    }

    makeAssignments( cost );

    return assignments;
  }
  
  void makeAssignments( RectangleArray< double > &cost )
  {
    assignments.clear();
    std::vector< bool > deletedRows( cost.getHeight(), false );
    std::vector< bool > deletedColumns( cost.getWidth(), false );

    bool makingAssignments = true;
    // Make as many assignments as possible
    while ( makingAssignments )
    {
      makingAssignments = false;

      bool madeAnotherAssignment = true;
      // Make as many unambiguous assignments as possible
      while ( madeAnotherAssignment )
      {
        madeAnotherAssignment = false;

        // Make assignments for each row with exactly one zero
        for ( int r = 0; r < (int)cost.getHeight(); r++ )
        {
          // Ignore deleted rows
          if ( !deletedRows[r] )
          {
            // Find the first and last zero in this row
            int firstZero = -1;
            int lastZero = -2;
            for ( int c = 0; c < (int)cost.getWidth(); c++ )
            {
              // Ignore deleted columns
              if ( !deletedColumns[c] )
              {
                if ( cost[r][c] == 0 )
                {
                  // We found a zero, update last zero
                  // and first zero if we haven't seen one yet
                  if ( firstZero == -1 )
                  {
                    firstZero = c;
                  }
                  lastZero = c;
                }
              }
            }

            // If there was only one zero, these will be the same
            if ( firstZero == lastZero )
            {
              // So make a new assignment
              assignments[r] = firstZero;
              madeAnotherAssignment = true;

              // And delete these rows and columns from the matrix
              deletedRows[r] = true;
              deletedColumns[firstZero] = true;

            }
          }
        }

        // Make assignments for each column with exactly one zero
        for ( int c = 0; c < (int)cost.getWidth(); c++ )
        {
          // Ignore deleted columns
          if ( !deletedColumns[c] )
          {
            // Find the first and last zero in this column
            int firstZero = -1;
            int lastZero = -2;
            for ( int r = 0; r < (int)cost.getHeight(); r++ )
            {
            // Ignore deleted rows
              if ( !deletedRows[r] )
              {
                if ( cost[r][c] == 0 )
                {
                  // We found a zero, update last zero
                  // and first zero if we haven't seen one yet
                  if ( firstZero == -1 )
                  {
                    firstZero = r;
                  }
                  lastZero = r;
                }
              }
            }

            // If there was only one zero, these will be the same
            if ( firstZero == lastZero )
            {
              // So make a new assignment
              assignments[firstZero] = c;
              madeAnotherAssignment = true;

              // And delete these rows and columns from the matrix
              deletedRows[firstZero] = true;
              deletedColumns[c] = true;
            }
          }
        }
      }
      // Make an ambiguous assignment
      for ( int r = 0; r < (int)cost.getHeight(); r++ )
      {
        if ( !deletedRows[r] )
        {
          for ( int c = 0; c < (int)cost.getWidth(); c++ )
          {
            if ( !deletedColumns[c] )
            {
              // Found a row with at least one zero
              if ( cost[r][c] == 0 )
              {
                // So make a new assignment
                assignments[r] = c;
                makingAssignments = true;

                // And delete these rows and columns from the matrix
                deletedRows[r] = true;
                deletedColumns[c] = true;
                break;
              }
            }
          }
        }
        if ( makingAssignments ) break;
      }
    }
  }
}