if(${verbose_traversal})
  message(STATUS "Entered Hayes subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB Hayes_h "*.h")
list( APPEND HEADER_FILES ${Hayes_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Hayes_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Hayes_h})
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
file(GLOB Hayes_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( Hayes ${${projectName}_policy} ${Hayes_src} )
set_property(TARGET Hayes PROPERTY CXX_STANDARD 11)
set_property(TARGET Hayes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Hayes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(Hayes EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting Hayes subdirectory")
endif(${verbose_traversal})
