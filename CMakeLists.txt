cmake_minimum_required(VERSION 3.6)

project("DasherUI")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/DasherUI)
add_compile_definitions(HAVE_ROUND)
find_package(OpenGL REQUIRED)

###############################
# Building GLFW Library
###############################

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_USE_HYBRID_HPG OFF)
set(GLFW_VULKAN_STATIC OFF)
add_subdirectory("Thirdparty/glfw")

###############################
# Building imgui Library
###############################

add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
ADD_library(imgui STATIC
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/imgui.h
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/imgui.cpp
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/imgui_draw.cpp 
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/imgui_tables.cpp 
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/misc/cpp/imgui_stdlib.cpp
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/backends/imgui_impl_glfw.cpp 
	${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Thirdparty/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Thirdparty/glfw/include)
add_dependencies(imgui glfw OpenGL::GL)

###############################
# Building Dasher Library
###############################

add_subdirectory("DasherCore")

###############################
# Misc Libs
###############################

set(NFD_BUILD_TESTS OFF)
set(NFD_INSTALL OFF)
add_subdirectory("Thirdparty/nativefiledialog-extended")

###############################
# DasherUI
###############################

IF (MSVC)
	# Hide console window
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
ENDIF()

ADD_Executable(DasherUI
	${PROJECT_SOURCE_DIR}/src/DasherController.cpp
	${PROJECT_SOURCE_DIR}/src/DasherController.h
	${PROJECT_SOURCE_DIR}/src/MainWindow.cpp
	${PROJECT_SOURCE_DIR}/src/MainWindow.h
	${PROJECT_SOURCE_DIR}/src/DasherUIScreen.cpp
	${PROJECT_SOURCE_DIR}/src/DasherUIScreen.h
	${PROJECT_SOURCE_DIR}/src/SocketInput.cpp
	${PROJECT_SOURCE_DIR}/src/SocketInput.h
	${PROJECT_SOURCE_DIR}/src/OSOutput.cpp
	${PROJECT_SOURCE_DIR}/src/OSOutput.h
	${PROJECT_SOURCE_DIR}/src/main.cpp
)
target_include_directories(DasherUI PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Thirdparty/)
add_dependencies(DasherUI imgui glfw DasherCore)
target_link_libraries(DasherUI PRIVATE imgui glfw OpenGL::GL DasherCore nfd)

###############################
# Third Party Includes
###############################
target_include_directories(DasherUI PRIVATE ${PROJECT_SOURCE_DIR}/Thirdparty/asio/asio/include/)

###############################
# Setting up deloyment
###############################

# Copy Data files
file(GLOB RESOURCE_FILES
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/alphabets/alphabet*.xml
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/colors/color*.xml
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/control/control*.xml
  ${PROJECT_SOURCE_DIR}/DasherCore/Data/training/training*.txt
)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/DasherUI/Data)

file(GLOB RESOURCE_FILES
  ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/misc/fonts/*.ttf
  ${PROJECT_SOURCE_DIR}/Resources/*.*
)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/DasherUI/Resources)

set_property(TARGET DasherUI PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/DasherUI/)

###############################
# Choose Startup Project
###############################

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DasherUI)

