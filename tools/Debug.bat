@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Debug /t:ImGuiRedux /property:Platform=Win32
del "%SA_DIR%\cleo\CLEO_PLUGINS\ImGuiReduxWin32.cleo" /Q
del "%SA_DIR%\cleo\CLEO_PLUGINS\ImGuiReduxWin32.pdb" /Q
xcopy /s "bin\ImGuiReduxWin32.cleo" "%SA_DIR%\cleo\CLEO_PLUGINS\" /K /D /H /Y 
xcopy /s "bin\ImGuiReduxWin32.pdb" "%SA_DIR%\cleo\CLEO_PLUGINS\" /K /D /H /Y 
