echo Creating Release and deploying to %1
xcopy .\include %1\include /e /i /h /y
xcopy .\external_libraries\BWAPI\bwapi\include %1\include /e /i /h /y
xcopy .\external_libraries\BWAPI\bwapi\lib %1\lib /e /i /h /y
xcopy .\external_libraries\BWTAlib_2.2\include %1\include /e /i /h /y
xcopy .\external_libraries\BWTAlib_2.2\lib %1\lib /e /i /h /y
xcopy .\lib\*.lib %1\lib /e /i /h /y
xcopy .\external_libraries\BWAPI\LICENSE %1\include\BWAPI\ /i /h /y
xcopy .\redistribution_notes\BWTA2_LICENSE\COPYING.txt %1\include\BWTA\ /i /h /y
xcopy .\redistribution_notes\README_LICENSE.txt %1\ /i /h /y
xcopy .\LICENSE.txt %1\ /i /h /y
echo Completed preparing release. Deployed in %1