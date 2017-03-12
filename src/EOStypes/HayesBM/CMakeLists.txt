if(${verbose_traversal})
  message(STATUS "Entered HayesBM subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB HayesBM_h "*.h")
list( APPEND HEADER_FILES ${HayesBM_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB HayesBM_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${HayesBM_h})
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
file(GLOB HayesBM_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( HayesBM-obj OBJECT ${HayesBM_src} )
  set_property(TARGET HayesBM-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET HayesBM-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET HayesBM-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET HayesBM-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( HayesBM ${${projectName}_policy}
                   $<TARGET_OBJECTS:HayesBM-obj> )
  set_property(TARGET HayesBM PROPERTY CXX_STANDARD 11)
  set_property(TARGET HayesBM PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET HayesBM PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries(HayesBM EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting HayesBM subdirectory")
endif(${verbose_traversal})

