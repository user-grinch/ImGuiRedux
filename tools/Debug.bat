@echo off
echo --------------------------------------------------
echo Building Debug
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Debug /t:ImGuiRedux
del %SA_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.cleo" /Q
del %SA_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.pdb" /Q
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.cleo" %SA_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.pdb" %SA_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
