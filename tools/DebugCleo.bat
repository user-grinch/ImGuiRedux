@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=DebugCleo /t:ImGuiRedux /property:Platform=Win32
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiCleoWin32.cleo" /Q
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiCleoWin32.pdb" /Q
xcopy /s "bin\ImGuiCleoWin32.cleo" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
xcopy /s "bin\ImGuiCleoWin32.pdb" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
