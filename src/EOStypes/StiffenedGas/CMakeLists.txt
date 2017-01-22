if(${verbose_traversal})
  message(STATUS "Entered StiffenedGas subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB StiffenedGas_h "*.h")
list( APPEND HEADER_FILES ${StiffenedGas_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB StiffenedGas_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${StiffenedGas_h})
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
file(GLOB StiffenedGas_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( StiffenedGas ${${projectName}_policy} ${StiffenedGas_src} )
set_property(TARGET StiffenedGas PROPERTY CXX_STANDARD 11)
set_property(TARGET StiffenedGas PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET StiffenedGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(StiffenedGas Eos)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting StiffenedGas subdirectory")
endif(${verbose_traversal})
