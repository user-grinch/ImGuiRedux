@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Release /t:ImGuiRedux
del %SA_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux.cleo" /Q
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.cleo" %SA_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
