@echo off

for /f "eol=# delims=" %%x in (..\Base.cfg) do (set "%%x")
call ..\Env\SetEnv-Win.cmd

cmd.exe