@echo off
echo.
echo ASCII Artinator test script started

if not exist aart.exe goto exeNotFound
goto run

:exeNotFound
echo aart.exe not found in current directory, attempting to use it on the path
goto run

:run
rem get version number
echo.
echo ---------------
for /F "delims=" %%i in ('aart -b') do set output=%%i
echo %output%
echo ---------------
echo.

echo Creating test files...
echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

aart -f --blockSize=8 images\CRW_0044.png tree.txt
aart -f -a -n --blockSize=16 images\test.png vangogh_small.txt
aart -f -i --blockSize=8 images\test.png vangogh_inverted.txt
aart -f --blockSize=4 images\test3.png mountains_detail.txt
aart -f -i --blockSize=4 images\US-wp6.png lakewallpaper-inverted.txt
aart -f --blockSize=8 images\US-wp6.png lakewallpaper.txt
echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
echo.
echo ASCII Artinator test script done