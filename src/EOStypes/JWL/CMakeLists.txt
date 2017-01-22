if(${verbose_traversal})
  message(STATUS "Entered JWL subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB JWL_h "*.h")
list( APPEND HEADER_FILES ${JWL_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB JWL_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${JWL_h})
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
file(GLOB JWL_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( JWL ${${projectName}_policy} ${JWL_src} )
set_property(TARGET JWL PROPERTY CXX_STANDARD 11)
set_property(TARGET JWL PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET JWL PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(JWL Eos)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting JWL subdirectory")
endif(${verbose_traversal})
