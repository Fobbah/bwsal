# bwsal
BWAPI Standard Add-on Library - Forked from the old repository on google code and upgraded to use the latest releases of BWAPI / BWTA

BWSAL has been upgraded to use:
- BWAPI 4.1.2: https://github.com/bwapi/bwapi/releases
- BWTA2: https://bitbucket.org/auriarte/bwta2
- Visual Studio 2013

# Installation Instructions (from source)
- Clone BWSAL
- Download BWTA2 2.2: https://bitbucket.org/auriarte/bwta2/downloads/BWTAlib_2.2.7z
    - Extract to BWSAL_DIRECTORY/external_libraries/BWTAlib_2.2/
  
- Download BWAPI 4.1.2 (source): https://github.com/bwapi/bwapi/archive/v4.1.2.zip
    - Extract to BWSAL_DIRECTORY/external_libraries/BWAPI/
  
- Open BWSAL_DIRECTORY/external_libraries/BWAPI.sln in VS2013
    - Compile in VS2013
  
- Open BWSAL_DIRECTORY/ExampleProjects.sln
    - Compile all, try out the example client modules - look to them as examples for your own client module using BWSAL