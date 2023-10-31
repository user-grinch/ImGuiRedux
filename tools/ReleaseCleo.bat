@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=ReleaseCleo /t:ImGuiRedux /property:Platform=Win32   
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiReduxWin32.cleo" /Q
xcopy /s "bin\ImGuiReduxWin32.cleo" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
