cmake_minimum_required(VERSION 3.10)
project(SmartKeyLogger)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ${CMAKE_SOURCE_DIR}/keylogger
    ${CMAKE_SOURCE_DIR}/analysis
    ${CMAKE_SOURCE_DIR}/ai  
)

add_executable(SmartKeyLogger
    main.cpp
    keylogger/hook.cpp
    keylogger/recorder.cpp
    analysis/analyzer.cpp
    ai/ai_model.cpp
    ai/spelling_corrector.cpp
)
