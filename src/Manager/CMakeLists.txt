if(${verbose_traversal})
  message(STATUS "Entered Manager subdirectory")
endif(${verbose_traversal})

message(STATUS "Manager")
#
# headers
#
file(GLOB Manager_h "*.h")
list( APPEND HEADER_FILES ${Manager_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Manager_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Manager_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)
#
# Source
#
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
file(GLOB Manager_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
add_library( Manager ${${projectName}_policy} ${Manager_src} )
set_property(TARGET Manager PROPERTY CXX_STANDARD 11)
set_property(TARGET Manager PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Manager PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( Manager ExtEOS )
#
# test
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting Manager subdirectory")
endif(${verbose_traversal})



