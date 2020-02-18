net use g: \\kpfiler1.intra.dlr.de\AT-KP
copy features\gitlab_windows_local_settings512.pri local_settings.pri

call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\AUXILIARY\\BUILD\\vcvarsall.bat" amd64

C:\\Qt\\5.12.6\\msvc2017_64\\bin\\qmake.exe gtlab.pro -spec win32-msvc "CONFIG+=debug"
nmake clean
C:\Qt\Tools\QtCreator\bin\jom.exe
