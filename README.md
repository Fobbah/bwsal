# BWSAL2 - BWAPI Standard Add-on Library ("V2")
Forked from the old repository on google code and upgraded to use the latest releases of BWAPI / BWTA2

BWSAL has been upgraded to use:
- BWAPI 4.1.2: https://github.com/bwapi/bwapi/releases
- BWTA2 2.2: https://bitbucket.org/auriarte/bwta2
- Visual Studio 2013

# Compilation Instructions (from source, automated setup)
- Clone BWSAL
- Run SETUP.bat (and wait for a few minutes)
- Open BWSAL_DIRECTORY/ExampleProjects.sln

# Compilation Instructions (from source, manual setup)
- Clone BWSAL
- Download BWTA2 2.2: https://bitbucket.org/auriarte/bwta2/downloads/BWTAlib_2.2.7z
    - Extract to BWSAL_DIRECTORY/external_libraries/BWTA2/
  
- Download BWAPI 4.1.2 (source): https://github.com/bwapi/bwapi/archive/v4.1.2.zip
    - Extract to BWSAL_DIRECTORY/external_libraries/BWAPI/
  
- Open BWSAL_DIRECTORY/external_libraries/BWAPI/bwapi/bwapi.sln in VS2013
    - Compile in VS2013
  
- Open BWSAL_DIRECTORY/ExampleProjects.sln
    - Compile all, try out the example client modules - look to them as examples for your own client module using BWSAL