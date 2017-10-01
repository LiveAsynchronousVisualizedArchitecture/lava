clang++ -std=c++14  -stdlib=libcmt -v -H -flto tiny_test.cpp -s -fno-rtti -fno-exceptions -ffunction-sections -funwind-tables  -O3 -c tiny_test.o
@echo -Compilation Finished-

lld-link.exe /libpath:"C:\\Program Files (x86)\\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.10.25017\lib\x64" /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\um\\x64"   /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\ucrt\\x64" /defaultlib:kernel32.lib /defaultlib:user32.lib  /subsystem:console /dll /out:tiny_test.dll libcmt.lib  tiny_test.o 

@echo -Link Stage Finished-
ls -lh

@rem libcmt.lib
@rem /entry:_entry
@rem libcmt.lib libcpmt.lib
@rem -fexceptions -fcxx-exceptions
@rem libucrt.lib
@rem -o tiny_test.o



