@echo off

for /f "eol=# delims=" %%x in (%AP_PATH%\Usr\Env\EnvVar.cfg) do (set "%%x")

rem Setup PATH variable
set PATH=%PATH%;%AP_PATH%\%GLEW_BIN_PATH%;%AP_PATH%\%DEVIL_BIN_PATH%;%AP_PATH%\%ASSIMP_BIN_PATH%

cd %AP_PATH%