@echo off
echo --------------------------------------------------
echo Building Release
echo --------------------------------------------------
echo[
call "tools\Setup.bat"
MsBuild ImGuiRedux.sln /property:Configuration=Release /t:ImGuiRedux
del %SA_DIR%"\ImGuiRedux.asi" /Q
%systemroot%\System32\xcopy /s "bin\ImGuiRedux.asi" %SA_DIR% /K /D /H /Y 
