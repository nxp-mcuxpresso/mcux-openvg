set OPENVG_ROOT=%cd%
set VGLITE_ROOT=%OPENVG_ROOT%\..\Hubi.dev
set CTS_ROOT=%OPENVG_ROOT%\..\..\..\TEST\SW\Khronos\Conformance\vg11\ovg_1.1_cts_rc12\cts\generation\make\win32\bin
set path=%OPENVG_ROOT%\BUILD;%VGLITE_ROOT%\BUILD_vgHuBi\bin;%path%

START "" "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe"  %OPENVG_ROOT%\openvg_only.sln
