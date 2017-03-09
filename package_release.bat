echo Creating Release Packages and deploying to %1
xcopy .\include %1\include /e /i /h /y
xcopy .\external_libraries\BWAPI\bwapi\include %1\include /e /i /h /y
xcopy .\external_libraries\BWAPI\bwapi\lib %1\lib /e /i /h /y
xcopy .\external_libraries\BWTAlib_2.2\include %1\include /e /i /h /y
xcopy .\external_libraries\BWTAlib_2.2\lib %1\lib /e /i /h /y
xcopy .\lib\*.lib %1\lib /e /i /h /y
