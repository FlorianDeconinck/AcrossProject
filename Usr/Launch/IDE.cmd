@echo off

for /f "eol=# delims=" %%x in (..\Base.cfg) do (set "%%x")
call ..\Env\SetEnv-Win.cmd

start /D %IDE_PATH% %IDE_EXE%
