@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Debug /t:ImGuiRedux
del %III_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.cleo" /Q
del %III_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.pdb" /Q
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.cleo" %III_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.pdb" %III_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
