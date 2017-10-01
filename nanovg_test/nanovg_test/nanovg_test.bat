clang++ -iquote "."  -I"H:\\projects\\lava\\nanogui\\include"  -std=c++14  -stdlib=libcmt -v -flto  -s -fno-rtti -fno-exceptions -ffunction-sections -funwind-tables  -O3 -c  unified.cpp
@echo -Compilation Finished-

lld-link.exe /libpath:"C:\\Program Files (x86)\\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.10.25017\lib\x64" /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\um\\x64"   /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\ucrt\\x64" /defaultlib:kernel32.lib /defaultlib:user32.lib  /subsystem:console /dll /out:fissure.exe libcmt.lib  unified.o 

@echo -Link Stage Finished-

