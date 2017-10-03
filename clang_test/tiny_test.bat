clang++ -fms-compatibility -fms-compatibility-version=19 -fms-extensions -std=c++14  -nostdinc -nostdinc++ -nostdlibinc -nobuiltininc  -I"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.10.25017\\include" -I"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.15063.0\\ucrt" -I"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.15063.0\\um" -I"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.15063.0\\shared"  -I"H:\\projects\\lava\\nanogui\\include" -I"H:\\projects\\lava\\nanogui\\ext\\eigen"  -w  -ferror-limit=10  -v -flto -fno-rtti -ffunction-sections -Oz tiny_test.cpp -c -o tiny_test.o
@echo -Compilation Finished-

lld-link.exe /libpath:"C:\\Program Files (x86)\\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.10.25017\lib\x64" /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\um\\x64"   /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.15063.0\\ucrt\\x64" /machine:x64 /subsystem:console /out:tiny_test.dll /dll   tiny_test.o 
@echo -Link Stage Finished-

ls -lh

@rem -stdlib=libcmt
@rem -system-header-prefix "/"  -iquote "."
@rem -std=c++14
@rem -fms-compatibility-version=19
@rem -internal-isystem "."
@rem /defaultlib:kernel32.lib /defaultlib:user32.lib
@rem libcmt.lib


