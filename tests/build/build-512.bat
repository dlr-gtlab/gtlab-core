net use g: \\kpfiler1.intra.dlr.de\AT-KP
copy features\gitlab_windows_local_settings512.pri local_settings.pri

set path=%path%;%qtdir%\bin;"C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.10.25017\bin\HostX64\x64"

C:\\Qt\\5.12.6\\msvc2017_64\\bin\\qmake.exe gtlab.pro -spec win32-msvc "CONFIG+=debug"
C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile qmake_all
C:\Qt\Tools\QtCreator\bin\jom.exe
