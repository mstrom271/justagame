# Just a game #
Tested under android, linux, windows

## Requirements ##
Qt, Conan, Android NDK, [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy)

### Script invoke examples: ###
```bash
./build.sh \
    OS=Linux \
    BUILD_TYPE=Debug \
    Qt6_DIR=~/Qt/6.5.1/gcc_64
```

```bash
./build.sh \
    OS=Android \
    BUILD_TYPE=Debug \
    Qt6_DIR=~/Qt/6.5.1/android_arm64_v8a \
    ANDROID_NDK=/opt/android-sdk/ndk/25.1.8937393/ \
    API_LEVEL=31 \
    ABI=armv8 \
    ANDROID_DEVICE_ID=28ff7904
```

### Required environment or commandline variables: ###
```bash
export STAGE=Build          # Config, Build, Deploy. If not specified, all three stages are implied

export OS=Android          # Android, Linux, Windows
export BUILD_TYPE=Debug    # Debug, Release
export Qt6_DIR=~/Qt/6.5.1/android_arm64_v8a  # Qt for target platform
```

```bash
# android specific variables:
export ANDROID_NDK=/opt/android-sdk/ndk/25.1.8937393/
export API_LEVEL=31
export ABI=armv8       # armv8, armv7, x86_64, x86
export ANDROID_DEVICE_ID=$(adb devices | sed -n 2p | awk '{print $1}')
```
