adb shell su -c killall BigWhiteHack
adb push outputs\arm64-v8a\BigWhiteHack /data/local/tmp
@REM adb push Z:\BigWhite\BigWhite.ko /data/local/tmp
adb shell su -c chmod 777 /data/local/tmp/BigWhiteHack
adb shell su -c /data/local/tmp/BigWhiteHack
