project(Launcher)

define_program("Bloodshot Engine Launcher")

target_include_directories(${PROJECT_NAME} PRIVATE ${BLOODSHOT_THIRD_PARTY_DIR}/stb_image)

target_link_libraries(${PROJECT_NAME} PRIVATE Shared)
target_link_libraries(${PROJECT_NAME} PRIVATE Core)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
