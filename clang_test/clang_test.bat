@rem @echo off
@rem @echo -Compiling-
clang++ clang_test.cpp -O3 -c -o clang_test.o
@echo -Compilation Finished-

@rem @echo -Linking-
lld-link.exe /entry:entry /subsystem:console /out:clang_test.exe clang_test.o 
@echo -Link Stage Finished-
ls -lh

