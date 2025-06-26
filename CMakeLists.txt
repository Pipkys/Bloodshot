cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

option(BS_LOGGING "Logging in engine" ON)
option(BS_PROFILING "Profiling in engine" ON)
option(BS_MULTITHREADING "Multithreading in engine" ON)
option(BS_WITH_AUTOMATION_TESTS "Automation tests in engine" ON)
option(BS_WITH_EDITOR "Builds engine with editor" ON)

project("Bloodshot Engine")

set(CMAKE_CXX_STANDARD 23)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_DEBUG_POSTFIX "")

#Common
set(BLOODSHOT_PROJECT_FOLDER_NAME ${PROJECT_NAME})
set(BLOODSHOT_CMAKE_MODULE_DIR ${CMAKE_SOURCE_DIR}/Shared/cmake)
set(BLOODSHOT_BINARIES_DIR ${CMAKE_SOURCE_DIR}/Binaries)
set(BLOODSHOT_INTERMEDIATE_DIR ${CMAKE_SOURCE_DIR}/Intermediate)
set(BLOODSHOT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)
set(BLOODSHOT_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/ThirdParty)
set(BLOODSHOT_PROGRAMS_DIR ${BLOODSHOT_SOURCE_DIR}/Programs)
set(BLOODSHOT_SHARED_DIR ${BLOODSHOT_SOURCE_DIR}/Shared)

#Modules
include(${BLOODSHOT_CMAKE_MODULE_DIR}/Macros.cmake)

#glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${BLOODSHOT_THIRD_PARTY_DIR}/glfw)
module_output_properties(glfw glfw ${BLOODSHOT_BINARIES_DIR})
target_include_directories(glfw PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)

#glad
add_subdirectory(${BLOODSHOT_THIRD_PARTY_DIR}/glad)
set_target_properties(glad PROPERTIES FOLDER glad)
module_output_properties(glad glad ${BLOODSHOT_BINARIES_DIR})
target_include_directories(glad PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glad/include)

#glm
add_subdirectory(${BLOODSHOT_THIRD_PARTY_DIR}/glm)
set_target_properties(glm PROPERTIES FOLDER glm)
module_output_properties(glm glm ${BLOODSHOT_BINARIES_DIR})
target_include_directories(glm PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/glm)

#stb_image
set(STB_IMAGE_HEADER ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image/stb_image.h)

#assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${BLOODSHOT_THIRD_PARTY_DIR}/assimp)
set_target_properties(assimp PROPERTIES FOLDER assimp)
module_output_properties(assimp assimp ${BLOODSHOT_BINARIES_DIR})

#imgui
set(IMGUI_IMPL_OPENGL3_SOURCE ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp)
set(IMGUI_IMPL_OPENGL3_HEADER ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.h)

set(IMGUI_IMPL_GLFW_SOURCE ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp)
set(IMGUI_IMPL_GLFW_HEADER ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.h)

file(GLOB IMGUI_SOURCES ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/*.cpp)
file(GLOB IMGUI_HEADERS ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/*.h)

add_library(imgui STATIC
	${IMGUI_SOURCES}
	${IMGUI_HEADERS}
	${IMGUI_IMPL_OPENGL3_SOURCE}
	${IMGUI_IMPL_OPENGL3_HEADER}
	${IMGUI_IMPL_GLFW_SOURCE}
	${IMGUI_IMPL_GLFW_HEADER})

set_target_properties(imgui PROPERTIES FOLDER imgui)
module_output_properties(imgui imgui ${BLOODSHOT_BINARIES_DIR})

target_include_directories(imgui PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/imgui)
target_include_directories(imgui PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/imgui/backends)
target_include_directories(imgui PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/glfw/include)

#ImGuizmo
file(GLOB IMGUIZMO_SOURCES ${BLOODSHOT_THIRD_PARTY_DIR}/ImGuizmo/*.cpp)
file(GLOB IMGUIZMO_HEADERS ${BLOODSHOT_THIRD_PARTY_DIR}/ImGuizmo/*.h)

add_library(ImGuizmo STATIC
	${IMGUIZMO_SOURCES}
	${IMGUIZMO_HEADERS})

set_target_properties(ImGuizmo PROPERTIES FOLDER ImGuizmo)
module_output_properties(ImGuizmo ImGuizmo ${BLOODSHOT_BINARIES_DIR})

target_include_directories(ImGuizmo PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/ImGuizmo)
target_include_directories(ImGuizmo PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/imgui)

#ImGuiFileDialog
add_subdirectory(${BLOODSHOT_THIRD_PARTY_DIR}/ImGuiFileDialog)
set_target_properties(ImGuiFileDialog PROPERTIES FOLDER ImGuiFileDialog)
module_output_properties(ImGuiFileDialog ImGuiFileDialog ${BLOODSHOT_BINARIES_DIR})

target_include_directories(ImGuiFileDialog PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/ImGuizmo)
target_include_directories(ImGuiFileDialog PUBLIC ${BLOODSHOT_THIRD_PARTY_DIR}/imgui)

#Preprocessing
execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}
	"${BLOODSHOT_BINARIES_DIR}/Bloodshot Header Tool.exe" 
	Sources:${BLOODSHOT_SOURCE_DIR}/Runtime
	Output:${BLOODSHOT_INTERMEDIATE_DIR}
	COMMENT "Run Bloodshot Header Tool on Runtime sources")

execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}
	"${BLOODSHOT_BINARIES_DIR}/Bloodshot Header Tool.exe" 
	Sources:${BLOODSHOT_SOURCE_DIR}/Editor
	Output:${BLOODSHOT_INTERMEDIATE_DIR}
	COMMENT "Run Bloodshot Header Tool on Editor sources")

#Shared
add_subdirectory(${BLOODSHOT_SHARED_DIR})

#Programs
add_subdirectory(${BLOODSHOT_PROGRAMS_DIR})

#Editor
if(${BS_WITH_EDITOR})
	add_subdirectory(${BLOODSHOT_SOURCE_DIR}/Editor)
endif()

#Runtime
add_subdirectory(${BLOODSHOT_SOURCE_DIR}/Runtime)

#Other
set(CMAKE_EXE_LINKER_FLAGS_DEBUG ${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup)
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Launch)
