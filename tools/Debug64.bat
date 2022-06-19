@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Debug /t:ImGuiRedux /property:Platform=Win64
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiReduxWin64.cleo" /Q
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiReduxWin64.pdb" /Q
xcopy /s "bin\ImGuiReduxWin64.cleo" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
xcopy /s "bin\ImGuiReduxWin64.pdb" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
