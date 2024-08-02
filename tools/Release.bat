@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Release /t:ImGuiRedux /property:Platform=Win32   
del "%GTA_SA_DIR%\cleo\CLEO_PLUGINS\ImGuiReduxWin32.cleo" /Q
xcopy /s "bin\ImGuiReduxWin32.cleo" "%GTA_SA_DIR%\cleo\CLEO_PLUGINS\" /K /D /H /Y 
