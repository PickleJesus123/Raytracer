cmake_minimum_required(VERSION 2.8)
project(ImageKit_305)

#set the name of the compiled program
set(TARGET_NAME "ImageDemo")

include(Image/SetEnv.cmake)

#add project source files
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET_NAME} ${COMMON_LIBS})

include(Image/PostCommand.cmake)
