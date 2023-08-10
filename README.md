# NosNet

This is the descendant of https://github.com/Noscka/Cpp-Networking

## Namings
DCHLS -> Dynamic Central Host Lookup Server </br>
DNICC -> Direct Non-Interfered Communications Client

## Qt Build command
```
configure.bat -release -static -static-runtime -no-pch -opensource -confirm-license -prefix "D:\Libraries\Qt\6.6.0\Build-Static" -skip webengine -nomake tests -nomake examples
cmake --build . --parallel
cmake --install
```
