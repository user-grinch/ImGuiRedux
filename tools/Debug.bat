@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Debug /t:ImGuiRedux /property:Platform=Win32
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.cleo" /Q
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.pdb" /Q
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.cleo" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.pdb" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
