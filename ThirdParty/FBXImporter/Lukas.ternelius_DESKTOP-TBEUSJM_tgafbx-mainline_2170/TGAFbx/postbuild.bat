@echo off
echo Moving files to Distribution Folder...
SETLOCAL
set libDir=%2Distribution\lib\%1
set incDir=%2\Distribution\include\
set fbxDll=%2\ThirdParty\FBXSDK\lib\release\libfbxsdk.dll
set outDir=%3

if not exist %libDir% mkdir %libDir%
if not exist %incDir% mkdir %incDir%

copy /Y Version.h %incDir% >nul
copy /Y Importer.h %incDir% >nul
copy /Y TgaFbxStructs.h %incDir% >nul
copy /Y TgaFbx.h %incDir% >nul
copy /Y %fbxDll% %libDir% >nul
copy /Y %outDir%TGAFbx.lib %libDir% >nul
copy /Y %outDir%TGAFbx.pdb %libDir% >nul

echo Headers and Lib files for TGAFbx have been copied to %2Distribution\%1