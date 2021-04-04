CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(EasiOS)

option(LINK_SHARED_ZED "Link with the ZED SDK shared executable" ON)

if (NOT LINK_SHARED_ZED AND MSVC)
    message(FATAL_ERROR "LINK_SHARED_ZED OFF : ZED SDK static libraries not available on Windows")
endif()

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 OLD)
	cmake_policy(SET CMP0015 OLD)
endif(COMMAND cmake_policy)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

#Optional output to SPI LCD screen. 
#This was only set up for the Jetson Nano/TX1 GPIO pin header, and with a specific
#2 inch LCD screen by WaveShare based on the ST7789VW chip.
#Note this will be totally ignored if in Windows. 
option(SPI_LCD "Output to SPI LCD screen (Jetson Nano/TX1 Only)" OFF)
option(GPIO_HOTKEYS "Input for GPIO hotkey buttons (Jetson Nano/TX1 Only)" OFF)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
SET(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

SET(EXECUTABLE_OUTPUT_PATH ".")

IF(NOT WIN32)
    add_definitions(-Wno-format-extra-args)
    SET(SPECIAL_OS_LIBS "pthread" "X11" "lpthread")
    SET(PLATFORM_SPECIFIC_LIBS "-lpthread")

    #Threading support.
    find_package(Threads)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

    #Check option to use SPI for an LCD screen. 
    if(SPI_LCD)
        add_definitions(-DSPI_OUTPUT)
    endif()
    if(GPIO_HOTKEYS)
        add_definitions(-DGPIO_HOTKEYS)
    endif()
ELSE()
    add_definitions(-DWINDOWS)
ENDIF()

find_package(ZED 3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)



include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})

FILE(GLOB_RECURSE SRC_FILES src/*.cpp)
FILE(GLOB_RECURSE HDR_FILES include/*.hpp)
FILE(GLOB_RECURSE HDR_FILES include/*.h)
FILE(GLOB_RECURSE SRC_IMAGES images/*.png)


ADD_EXECUTABLE(${PROJECT_NAME} ${HDR_FILES} ${SRC_FILES} ${SRC_IMAGES})
add_definitions(-std=c++14 -O3)

source_group("Images" FILES ${SRC_IMAGES})

if (LINK_SHARED_ZED)
    SET(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY})
else()
    SET(ZED_LIBS ${ZED_STATIC_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_LIBRARY})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ZED_LIBS} ${OpenCV_LIBRARIES})
