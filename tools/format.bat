@echo off

if exist "format.bat" (
  cd ..
)

call clang-format -i --style=file src/*.cpp src/*.h include/cfd/*.h src/capi/*.cpp src/capi/*.h include/cfdc/*.h
