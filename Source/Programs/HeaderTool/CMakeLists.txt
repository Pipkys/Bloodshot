project(HeaderTool)

define_program("Bloodshot Header Tool")

target_link_libraries(${PROJECT_NAME} PRIVATE Shared)
target_link_libraries(${PROJECT_NAME} PRIVATE Core)
