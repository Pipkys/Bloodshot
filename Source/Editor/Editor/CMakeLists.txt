project(Editor)

define_shared_module()

target_link_libraries(${PROJECT_NAME} PUBLIC Assets)
target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Input)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
target_link_libraries(${PROJECT_NAME} PUBLIC World)
target_link_libraries(${PROJECT_NAME} PUBLIC Shared)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGuizmo)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGuiFileDialog)