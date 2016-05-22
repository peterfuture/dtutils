SET(BUILD_FOR_ANDROID yes)

# cross compile
SET(CMAKE_SYSTEM_NAME Linux) 
SET(TOOLCHAIN_DIR "/opt/toolchains/android-ndk-r11c-android-19-arm32-linux-androideabi-4.9")
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-androideabi-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-androideabi-g++)


