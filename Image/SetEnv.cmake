message( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
message( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )

if(APPLE)
    message("Setting compilation of Image Kit on OSX...")
    add_definitions(-DAPPLE_COMPILE)
	#add opengl
	find_package(OpenGL REQUIRED)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})

	add_definitions(-DWITH_OPENGL)
	list(APPEND COMMON_LIBS ${OPENGL_LIBRARIES})

	#add cocoa
	find_library(COCOA_LIBRARY Cocoa)
	list(APPEND COMMON_LIBS ${COCOA_LIBRARY})

	#add IOKit
	find_library(IOKIT_LIBRARY IOKit)
	list(APPEND COMMON_LIBS ${IOKIT_LIBRARY})

	#add CoreVideo
	find_library(CoreVideo_LIBRARY CoreVideo)
	list(APPEND COMMON_LIBS ${CoreVideo_LIBRARY})

    #add glfw -- use the local library file

	find_library(GLFW3_LIBRARIES NAMES libglfw3.a
	        PATHS ${CMAKE_SOURCE_DIR}/GLFW/lib/OSX)

	if(GLFW3_LIBRARIES)
	list(APPEND COMMON_LIBS ${GLFW3_LIBRARIES})
	else()
          message("Error: Cannot find local GLFW3 library (libglfw3.a) in /GLFW/lib/OSX.")
	endif()

    #add libpng -- use the local library file

    find_library(LIBPNG16_LIBRARY NAMES libpng.a
            PATHS ${CMAKE_SOURCE_DIR}/libpng/lib/OSX)
    if(LIBPNG16_LIBRARY)
    list(APPEND COMMON_LIBS ${LIBPNG16_LIBRARY})
    else()
      message("Error: Caanot find local libpng library (libpng.a) in /libpng/lib/OSX.")
    endif()

    find_library(LIBZ_LIBRARY NAMES libz.a
            PATHS ${CMAKE_SOURCE_DIR}/libpng/lib/OSX)
    if(LIBZ_LIBRARY)
    list(APPEND COMMON_LIBS ${LIBZ_LIBRARY})
    else()
      message("Error: Caanot find local libz compression library (libz.a) in /libpng/lib/OSX.")
    endif()

elseif(WIN32)
	message("Setting compilation of Image Kit on Win32...")
	add_definitions(-DWIN_COMPILE)

	#get rid of unused windows default libs
	#SET(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:\"libc.lib libcd.lib libcmt.lib libcmtd.lib msvcrt.lib\"")

	find_package(OpenGL REQUIRED)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})
	if(NOT OPENGL_FOUND) 
		message(ERROR " OPENGL not found!(win32)")
	else()
		add_definitions(-DWITH_OPENGL)
		list(APPEND COMMON_LIBS ${OPENGL_LIBRARIES})
	endif()

	#add glfw -- use the local library file
	find_library(GLFW3_LIBRARIES NAMES glfw3.lib
	        PATHS ${CMAKE_SOURCE_DIR}/GLFW/lib/win32)

	if(GLFW3_LIBRARIES)
	list(APPEND COMMON_LIBS ${GLFW3_LIBRARIES})
	else()
          message("Error: Cannot find local GLFW3 library (glfw3.lib) in /GLFW/lib/win32.")
	endif()

	#add glew -- otherwise we will only have openGL1.1
	find_library(GLEW_LIBRARIES NAMES glew32.lib 
		        PATHS ${CMAKE_SOURCE_DIR}/GLFW/lib/win32)

	if(GLEW_LIBRARIES)
	list(APPEND COMMON_LIBS ${GLEW_LIBRARIES})
	else()
          message("Error: Cannot find local GLEW library (glew32.lib) in /GLFW/lib/win32.")
	endif()


	#add libpng -- use the local library file
    find_library(LIBPNG16_LIBRARY NAMES libpng16_static.lib
            PATHS ${CMAKE_SOURCE_DIR}/libpng/lib/win32)
    if(LIBPNG16_LIBRARY)
    list(APPEND COMMON_LIBS ${LIBPNG16_LIBRARY})
    else()
      message("Error: Caanot find local libpng library (libpng16_static.lib) in /libpng/lib/win32.")
    endif() 

    #add zlib win32 version
    find_library(ZLIBSTATIC_LIBRARY NAMES zlibstatic.lib
            PATHS ${CMAKE_SOURCE_DIR}/libpng/lib/win32)
    if(ZLIBSTATIC_LIBRARY)
    list(APPEND COMMON_LIBS ${ZLIBSTATIC_LIBRARY})
    else()
      message("Error: Caanot find local zlib library (zlibstatic.lib) in /libpng/lib/win32.")
    endif() 

else()
#maybe we should fix this?
    message("I don't know how to compile beyond apple and win32.")
endif()

#add include directories
include_directories(${CMAKE_SOURCE_DIR}/libpng/include)
include_directories(${CMAKE_SOURCE_DIR}/GLFW/include)

#add the image class include path
include_directories(${CMAKE_SOURCE_DIR}/Image)