cmake_minimum_required(VERSION 3.18.1)
project(ultimate_engine)

set(CMAKE_CXX_STANDARD 17)

# AI engine sources
set(AI_SOURCES
    ai/ai_engine.cpp
    ai/tflite_runner.cpp
    ai/nnapi_runner.cpp
)

add_library(ultimate_engine SHARED
    engine.cpp
    jni_bridge.cpp
    ${AI_SOURCES}
)

find_library(log-lib log)
find_library(android-lib android)
find_library(gameactivity-lib gameactivity)

# TensorFlow Lite configuration
option(USE_TFLITE "Use TensorFlow Lite" OFF)
if(USE_TFLITE)
    find_package(TensorFlowLite REQUIRED)
    target_link_libraries(ultimate_engine TensorFlowLite::tensorflowlite)
    target_compile_definitions(ultimate_engine PUBLIC ENABLE_TFLITE=1)
else()
    target_compile_definitions(ultimate_engine PUBLIC ENABLE_TFLITE=0)
endif()

target_include_directories(ultimate_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ultimate_engine
    ${log-lib}
    ${android-lib}
    ${gameactivity-lib}
)
