## vitaTrak - 2023 @ thexkey
## This is a simple tracker music player for the PSVITA
## Recommended to use CLion to build this project.

cmake_minimum_required(VERSION 3.22)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
        message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
endif()

# check for CLion
if(NOT DEFINED ENV{CLION_IDE})
   message(WARNING "CLion not detected, building workflow may not work as expected.")
endif()

# you can edit the project name here
project(vitaTrak)

include("${VITASDK}/share/vita.cmake" REQUIRED)

# TODO: migrate back to C++ 11, but for now we need to use C++ 17
#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(VITA_APP_NAME "vitaTrak")
set(VITA_TITLEID  "VITATRAK1")
set(VITA_VERSION  "01.00")

add_executable(${PROJECT_NAME}
        src/main.cpp

        # openmpt
        src/openmpt/playback.cpp

        # imgui UI
        src/imgui/imgui.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_impl_vita2d.cpp
        src/imgui/imgui_vita_touch.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_demo.cpp
        )

# add the include directories
include_directories(${PROJECT_NAME}
        headers
        headers/imgui
        headers/openmpt
        )

target_link_libraries(${PROJECT_NAME}
        # cmake library loading is in reverse (top last, bottom first)

        openmpt
        # the dependencies of openmpt?
        z
        vorbisfile
        vorbis
        ogg
        mpg123
        # imgui UI + dependencies
        imgui_vita2d
        vita2d

        # system libraries
        vitaGL
        c
        SceCommonDialog_stub
        SceLibKernel_stub
        SceDisplay_stub
        SceGxm_stub
        SceSysmodule_stub
        SceCtrl_stub
        SceTouch_stub
        m
        SceAppMgr_stub
        SceAppUtil_stub
        mathneon
        SceKernelDmacMgr_stub
        vitashark
        SceShaccCgExt
        taihen_stub 
        SceShaccCg_stub
        SceAudio_stub
        SceAudiodec_stub
        stdc++
        pthread

        )

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}Ui.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE art/livearea/icon0-fs8.png sce_sys/icon0.png
        FILE art/livearea/contents/bg-fs8.png sce_sys/livearea/contents/bg.png
        FILE art/livearea/contents/startup-fs8.png sce_sys/livearea/contents/startup.png
        FILE art/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
        )