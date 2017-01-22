if(${verbose_traversal})
  message(STATUS "Entered HEburn subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB HEburn_h "*.h")
list( APPEND HEADER_FILES ${HEburn_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB HEburn_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${HEburn_h})
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
file(GLOB HEburn_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( HEburn ${${projectName}_policy} ${HEburn_src} )
set_property(TARGET HEburn PROPERTY CXX_STANDARD 11)
set_property(TARGET HEburn PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET HEburn PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(HEburn ExtEOS PTequilibrium)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting HEburn subdirectory")
endif(${verbose_traversal})
