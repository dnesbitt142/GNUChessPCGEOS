@echo off
rem Windows launcher. The PC/GEOS Windows SDK already requires Perl.
perl "%~dp0omf_normalize.pl" %*
exit /b %errorlevel%
