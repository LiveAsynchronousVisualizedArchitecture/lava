clang++ -flto -std=c++14 -v clang_test.cpp -O3 -c -o clang_test.o
@echo -Compilation Finished-

lld-link.exe /libpath:"C:\\Program Files (x86)\\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.10.25017\lib\x64" /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\um\\x64"   /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\ucrt\\x64"  /subsystem:console /out:clang_test.exe libcmt.lib clang_test.o 

@echo -Link Stage Finished-
ls -lh

@rem @echo off
@rem @echo -Compiling-
@rem @echo -Linking-
@rem clang++ -std=c++14 -v clang_test.cpp -O3 -o clang_test.o
@rem lld-link.exe /entry:entry /subsystem:console /out:clang_test.exe clang_test.o 
@rem /entry:entry
@rem /libpath:"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\lib\\amd64"
@rem /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\um\\x64"
@rem -defaultlib:libcmt
@rem /debugtype:none
