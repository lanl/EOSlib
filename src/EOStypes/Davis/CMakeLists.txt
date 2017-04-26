if(${verbose_traversal})
  message(STATUS "Entered Davis subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB Davis_h "*.h")
list( APPEND HEADER_FILES ${Davis_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Davis_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Davis_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)
#
# SOURCE
#
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
file(GLOB Davis_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( Davis-obj OBJECT ${Davis_src} )
  set_property(TARGET Davis-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET Davis-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET Davis-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET Davis-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( Davis ${${projectName}_policy}
                   $<TARGET_OBJECTS:Davis-obj> )
  set_property(TARGET Davis PROPERTY CXX_STANDARD 11)
  set_property(TARGET Davis PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET Davis PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries(Davis EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting Davis subdirectory")
endif(${verbose_traversal})
