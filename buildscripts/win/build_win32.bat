@set OUT_DIR=Debug
@set OUT_EXE=CEditor_win32
@set INCLUDES=/I..\..\inc\ruleset.h /I..\..\inc\ui.h /I..\..\inc\jsonhelper.h /I..\..\inc\dataformat.h /I..\..\vendor\imgui /I..\..\vendor\imgui\backends /I "%WindowsSdkDir%Include\um" /I "%WindowsSdkDir%Include\shared" /I "%DXSDK_DIR%Include"
@set SOURCES=..\..\src\main.cpp ..\..\src\ui.cpp ..\..\vendor\imgui\backends\imgui_impl_dx11.cpp ..\..\vendor\imgui\backends\imgui_impl_win32.cpp ..\..\vendor\imgui\imgui*.cpp
@set LIBS=/LIBPATH:"%DXSDK_DIR%/Lib/x86" d3d11.lib d3dcompiler.lib
mkdir %OUT_DIR%
cl /nologo /Zi /MD /utf-8 %INCLUDES% /D UNICODE /D _UNICODE %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

