@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Release /t:ImGuiRedux /property:Platform=Win64
del "%SA_DIR%\cleo\CLEO_PLUGINS\ImGuiReduxWin64.cleo" /Q
xcopy /s "bin\ImGuiReduxWin64.cleo" "%SA_DIR%\cleo\CLEO_PLUGINS\" /K /D /H /Y 
