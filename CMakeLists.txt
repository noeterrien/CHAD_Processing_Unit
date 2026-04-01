cmake_minimum_required(VERSION 3.1)

project(CPU)

find_package(OpenCV 4.2 REQUIRED)
find_package(nadjieb_mjpeg_streamer REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(CPU)
target_sources(CPU
    PRIVATE
        main.cpp
        utilitaries.cpp)
target_compile_features(CPU 
    PRIVATE 
        cxx_std_17)
target_link_libraries(CPU 
    PRIVATE 
        nadjieb_mjpeg_streamer::nadjieb_mjpeg_streamer
        ${OpenCV_LIBS}
        Display
        Camera
        FrameProcessor
        Interface    
)


add_library(Display)
target_sources(Display
    PRIVATE
        Display.cpp
        
    PUBLIC
        FILE_SET HEADERS
        FILES
            Display.hpp)

            
add_library(Camera)
target_sources(Camera
    PRIVATE
        Camera.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            Camera.hpp
)
target_link_libraries(Camera
    PRIVATE
        ${OpenCV_LIBS}
)


add_library(FrameProcessor)
target_sources(FrameProcessor
    PRIVATE
        FrameProcessor.cpp

    PUBLIC
        FILE_SET HEADERS
        FILES
            FrameProcessor.hpp
)
target_link_libraries(FrameProcessor
    PUBLIC
        ${OpenCV_LIBS}
)


add_library(Interface)
target_sources(Interface
    PRIVATE
        Interface.cpp
        
    PUBLIC
        FILE_SET HEADERS
        FILES
            Interface.hpp)