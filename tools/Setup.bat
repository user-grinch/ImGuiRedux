rem Generate visual studio files and run the devcmd init
@echo off

rem Set game paths below
set "OUT_DIR="D:\\Games\\Grand.Theft.Auto.The.Trilogy.The.Definitive.Edition.v1.04.5\\Grand.Theft.Auto.San.Andreas\\Gameface\\Binaries\\Win64""

cd tools
premake5.exe vs2022
cd ../build
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
