if(${verbose_traversal})
  message(STATUS "Entered GenHayes subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB GenHayes_h "*.h")
list( APPEND HEADER_FILES ${GenHayes_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB GenHayes_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${GenHayes_h})
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
file(GLOB GenHayes_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( GenHayes ${${projectName}_policy} ${GenHayes_src} )
set_property(TARGET GenHayes PROPERTY CXX_STANDARD 11)
set_property(TARGET GenHayes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET GenHayes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(GenHayes EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting GenHayes subdirectory")
endif(${verbose_traversal})
