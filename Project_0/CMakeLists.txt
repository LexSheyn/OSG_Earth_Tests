cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(project_0 VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE SOURCES "source/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

# Hardcoded include directories!
target_include_directories(${PROJECT_NAME} PRIVATE usr/local/include)
target_include_directories(${PROJECT_NAME} PRIVATE usr/include)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Hardcoded library directories!
target_link_directories(${PROJECT_NAME} PRIVATE usr/local/lib)
target_link_directories(${PROJECT_NAME} PRIVATE usr/local/lib64)
target_link_directories(${PROJECT_NAME} PRIVATE usr/lib/x86_64-linux-gnu)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenThreads)
target_link_libraries(${PROJECT_NAME} PRIVATE osg)
target_link_libraries(${PROJECT_NAME} PRIVATE osgDB)
target_link_libraries(${PROJECT_NAME} PRIVATE osgUtil)
target_link_libraries(${PROJECT_NAME} PRIVATE osgViewer)
target_link_libraries(${PROJECT_NAME} PRIVATE osgGA)
target_link_libraries(${PROJECT_NAME} PRIVATE osgEarth)
target_link_libraries(${PROJECT_NAME} PRIVATE glut)
target_link_libraries(${PROJECT_NAME} PRIVATE GL)
