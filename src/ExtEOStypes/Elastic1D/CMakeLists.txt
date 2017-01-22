if(${verbose_traversal})
  message(STATUS "Entered Elastic1D subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB Elastic1D_h "*.h")
list( APPEND HEADER_FILES ${Elastic1D_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Elastic1D_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Elastic1D_h})
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
file(GLOB Elastic1D_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( Elastic1D ${${projectName}_policy} ${Elastic1D_src} )
set_property(TARGET Elastic1D PROPERTY CXX_STANDARD 11)
set_property(TARGET Elastic1D PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Elastic1D PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(Elastic1D ExtEOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting Elastic1D subdirectory")
endif(${verbose_traversal})
