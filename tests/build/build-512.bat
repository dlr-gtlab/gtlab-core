copy features\gitlab_pipeline_local_settings.pri local_settings.pri

call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\AUXILIARY\\BUILD\\vcvarsall.bat" amd64

C:\\Qt\\Qt5.12.7\\5.12.7\\msvc2017_64\\bin\\qmake.exe %PROFILENAME% -spec win32-msvc "CONFIG+=%BUILDMODE%"
nmake clean
C:\Qt\Qt5.12.7\Tools\QtCreator\bin\jom.exe
