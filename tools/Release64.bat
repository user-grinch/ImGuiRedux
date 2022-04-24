@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Release /t:ImGuiRedux64
del %OUT_DIR%"\cleo\CLEO_PLUGINS\ImGuiRedux64.cleo" /Q
%systemroot%\System32\xcopy /s "bin\ImGuiRedux64.cleo" %OUT_DIR%"\cleo\CLEO_PLUGINS\" /K /D /H /Y 
