@echo off

for /f "eol=# delims=" %%x in (..\Base.cfg) do (set "%%x")
call ..\Env\SetEnv-Win.cmd

start /D %VC12_PATH% %VC12_EXE%
