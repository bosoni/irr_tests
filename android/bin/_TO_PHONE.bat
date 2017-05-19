call adb install -s -r %1
call adb logcat -c
call adb logcat Irrlicht:V DEBUG:V *:S
