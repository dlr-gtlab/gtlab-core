REM TEST FOR CODINGSTYLE
SET PROGRAMPATH="C:\Program Files"
%PROGRAMPATH%\nsiqcppstyle_0.2.2.3\nsiqcppstyle\nsiqcppstyle --ci --output=xml -o .\ -f  .\features\filefilter.txt .\src
			