 if(WIN32)
 	#copy the dll file to the executable directory
    message("Setting up copy glew dll..." "${CMAKE_BINARY_DIR}/Debug/")
    add_custom_command(TARGET ${TARGET_NAME}
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${CMAKE_SOURCE_DIR}/GLFW/lib/win32/glew32.dll"      
        "${CMAKE_BINARY_DIR}/Debug/")
endif()