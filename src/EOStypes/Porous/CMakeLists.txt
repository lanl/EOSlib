if(${verbose_traversal})
  message(STATUS "Entered Porous subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB Porous_h "*.h")
list( APPEND HEADER_FILES ${Porous_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Porous_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Porous_h})
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
file(GLOB Porous_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( Porous ${${projectName}_policy} ${Porous_src} )
set_property(TARGET Porous PROPERTY CXX_STANDARD 11)
set_property(TARGET Porous PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Porous PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(Porous EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting Porous subdirectory")
endif(${verbose_traversal})

