cd /d "%~dp0"
set astyle=%~dp0AStyle.exe --style=linux --s4 --p --H --U --f --v --w --c --xe --xL --xW
set dir_path=".\src"
echo "format source code......"
for /R %dir_path% %%a in (*.cpp;*.c;*.cc;*.h;*.hpp) do %astyle% "%%a" 1>nul 2>nul
echo "delete backup source code......"
for /R %dir_path% %%a in (*.orig) do del "%%a"
echo "format source code end."