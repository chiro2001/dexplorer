@echo off

set root_dir=.\build\DISK_C

if "%1" == "" (
  make build
  @REM echo Commands: clean copy start build
  goto end
)

if "%1" == "clean" (
  if exist %root_dir%\DExp (
    echo deleting file: %root_dir%\DExp
    rd /q /s %root_dir%\DExp
  )
  goto end
)

if "%1" == "copy" (
  echo copying .\DExplorer to %root_dir%\DExp
  xcopy .\DExplorer %root_dir%\DExp /e /Y /d /i
  goto end
)

if "%1" == "start" (
  make clean
  make copy
  start .\build\dosbox\dosbox.exe -conf scripts\start.conf -noconsole
  goto end
)

if "%1" == "build" (
  make clean
  make copy
  start .\build\dosbox\dosbox.exe -conf scripts\build.conf -noconsole
  goto end
)

:end