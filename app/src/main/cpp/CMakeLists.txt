#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native-activity SHARED ${CMAKE_SOURCE_DIR}/main.cpp)

target_include_directories(native-activity PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue)



add_library( node STATIC IMPORTED )
set_target_properties( node PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/libnode.a/libnode.a )

add_library( skia STATIC IMPORTED )
set_target_properties( skia PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-canvas-deps/lib2/android/libskia.a )

add_library( LabSound STATIC IMPORTED )
set_target_properties( LabSound PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-audio-deps/lib2/android/libLabSound.a )

add_library( avformat STATIC IMPORTED )
set_target_properties( avformat PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libavformat.a )
add_library( avcodec STATIC IMPORTED )
set_target_properties( avcodec PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libavcodec.a )
add_library( avutil STATIC IMPORTED )
set_target_properties( avutil PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libavutil.a )
add_library( avdevice STATIC IMPORTED )
set_target_properties( avdevice PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libavdevice.a )
add_library( swscale STATIC IMPORTED )
set_target_properties( swscale PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libswscale.a )
add_library( swresample STATIC IMPORTED )
set_target_properties( swresample PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libswresample.a )
add_library( opus STATIC IMPORTED )
set_target_properties( opus PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-video-deps/lib2/android/libopus.a )

add_library( webrtc STATIC IMPORTED )
set_target_properties( webrtc PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-webrtc-deps/lib/android/libwebrtc.a )
add_library( OpenSLES SHARED IMPORTED )
set_target_properties( OpenSLES PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/node_modules/native-webrtc-deps/lib/android/libOpenSLES.so )


include_directories( SYSTEM ${CMAKE_SOURCE_DIR}/node_modules/nan )
include_directories( SYSTEM ${CMAKE_SOURCE_DIR}/node_nodules/exokit/deps/exokit-bindings/util/include )
include_directories( SYSTEM ${CMAKE_SOURCE_DIR}/.node-gyp/11.10.1/include/node )


# add lib dependencies
target_link_libraries(native-activity
    android
    native_app_glue
    EGL
    GLESv1_CM
    log
    node
    skia
    LabSound
    avformat
    avcodec
    avutil
    avdevice
    swscale
    swresample
    opus
    webrtc
    OpenSLES
        )