project(World)

define_shared_module()

target_link_libraries(${PROJECT_NAME} PUBLIC Assets)
target_link_libraries(${PROJECT_NAME} PUBLIC Core)
target_link_libraries(${PROJECT_NAME} PUBLIC Rendering)
