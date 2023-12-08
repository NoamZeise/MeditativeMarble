add_executable(bmjam
  main.cpp
  third_person_cam.cpp
  model_gen.cpp
  debug.cpp)
target_link_options(bmjam PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})
target_link_libraries(bmjam GraphicsEnv)

set(exec-name bmjam)
include("${CMAKE_CURRENT_SOURCE_DIR}/../Graphics-Environment/cmake/copy-resources.cmake")
# copy bmjam resource folder to executable folder
add_custom_command(TARGET bmjam POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_CURRENT_SOURCE_DIR}/../resources"
  $<TARGET_FILE_DIR:bmjam>)
