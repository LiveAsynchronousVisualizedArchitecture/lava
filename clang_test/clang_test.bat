@rem @echo off
@rem @echo -Compiling-
clang++ -std=c++14 -v clang_test.cpp -O3 -c -o clang_test.o
@echo -Compilation Finished-

@rem @echo -Linking-
lld-link.exe /libpath:"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\lib\\amd64" /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\um\\x64"  -defaultlib:libcmt  /subsystem:console /out:clang_test.exe clang_test.o 

@echo -Link Stage Finished-
ls -lh

@rem clang++ -std=c++14 -v clang_test.cpp -O3 -o clang_test.o
@rem lld-link.exe /entry:entry /subsystem:console /out:clang_test.exe clang_test.o 
@rem /entry:entry


