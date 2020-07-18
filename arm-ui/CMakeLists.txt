cmake_minimum_required(VERSION 3.5)

project(arm-ui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Library setup
set(Qt5_DIR "C:\\Qt\\5.9.9\\msvc2017_64\\lib\\cmake\\Qt5")
set(MSGPACK_CXX17 ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")

#set(CMAKE_PREFIX_PATH "C:\\Qt\\5.9.9\\msvc2017_64\\")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets SerialPort REQUIRED)

add_subdirectory(include/json)

include_directories(include/msgpack-c/include)

if(ANDROID)
  add_library(arm-ui SHARED
    arm-ui
    main.cpp
    mainwindow.cpp
    mainwindow.h
    joint.cpp
    joint.h
    joint_graphic.cpp
    joint_graphic.h
    angle.cpp
    angle.h
    position.cpp
    position.h
    geometry.h
    mainwindow.ui
          )
else()
  add_executable(
    arm-ui
    main.cpp
    mainwindow.cpp
    mainwindow.h
    joint.cpp
    joint.h
    joint_graphic.cpp
    joint_graphic.h
    angle.cpp
    angle.h
    position.cpp
    position.h
    geometry.h
    mainwindow.ui
          arc.cpp arc.h)
endif()

target_link_libraries(arm-ui PRIVATE nlohmann_json::nlohmann_json Qt5::Widgets Qt5::SerialPort)
