@echo Try to copy  libIrrlicht.a  to this directory (modify path if necassary)
call copy E:\CPP\irrlicht\irrlicht-ogl-es\lib\Android\libIrrlicht.a .
pause

@echo Copy sources...
call copy ..\src src


path=%PATH%;E:\ANDROID_kamaa\android-ndk-r9b
call ndk-build
pause
call ant debug
pause

rem call adb install -s -r bin\TestIrr-debug.apk
rem call adb logcat -c
rem call adb logcat Irrlicht:V *:S
