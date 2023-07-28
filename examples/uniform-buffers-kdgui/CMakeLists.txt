project(uniform-buffers-kdgui)

find_package(assimp REQUIRED)

add_executable(${PROJECT_NAME}
	entry_main.cpp "window.h"
 "window.cpp" "app.h" "app.cpp")

set(LOCAL_RESOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${LOCAL_RESOURCE_DIR})

if(WIN32)
	include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/Functions.cmake)
	wis_install_dx(${PROJECT_NAME})
endif()

target_link_libraries(
	${PROJECT_NAME}
		PUBLIC Wisdom glm::glm PRIVATE KDFoundation KDGui
)

target_link_libraries(${PROJECT_NAME} PUBLIC assimp::assimp)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
)
target_compile_definitions(${PROJECT_NAME} 
	PUBLIC 
		SHADER_DIR="${SHADER_DIR}"
        LOCAL_RESOURCE_DIR="${LOCAL_RESOURCE_DIR}"
		GLM_FORCE_SIMD_AVX2
		GLM_FORCE_ALIGNED_GENTYPES
)

if(UNIX)
	target_link_options(${PROJECT_NAME} 
		PRIVATE 
			"-lm"
	)
endif()

add_dependencies(${PROJECT_NAME}
    compile_shaders_kd
)
