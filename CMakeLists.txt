cmake_minimum_required(VERSION 3.1)

project(CPU)

find_package(OpenCV 4.2 REQUIRED)
find_package(nadjieb_mjpeg_streamer REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(CPU)
target_sources(CPU
    PRIVATE
        main.cpp
        libraries/utilitaries.cpp)
target_compile_features(CPU 
    PRIVATE 
        cxx_std_17)
target_link_libraries(CPU 
    PRIVATE 
        ${OpenCV_LIBS}
        Camera
        FrameProcessor
        Interface  
        Parameters  
)

            
add_library(Camera)
target_sources(Camera
    PRIVATE
        libraries/Camera.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS 
            libraries
        FILES
            libraries/Camera.hpp
)
target_link_libraries(Camera
    PRIVATE
        ${OpenCV_LIBS}
)


add_library(FrameProcessor)
target_sources(FrameProcessor
    PRIVATE
        libraries/FrameProcessor.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS 
            libraries
        FILES
            libraries/FrameProcessor.hpp
)
target_link_libraries(FrameProcessor
    PUBLIC
        ${OpenCV_LIBS}
)


add_library(Interface)
target_sources(Interface
    PRIVATE
        libraries/Interface.cpp
        
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS 
            libraries
        FILES
            libraries/Interface.hpp)

add_library(Parameters)
target_sources(Parameters
    PRIVATE
        libraries/Parameters.cpp
        
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS 
            libraries
        FILES
            libraries/Parameters.hpp)