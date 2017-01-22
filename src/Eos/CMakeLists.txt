if(${verbose_traversal})
  message(STATUS "Entered Eos subdirectory")
endif(${verbose_traversal})

message(STATUS "Eos")
#
# headers
#
file(GLOB Eos_h "*.h")
list( APPEND HEADER_FILES ${Eos_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Eos_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Eos_h})
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
file(GLOB Eos_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
add_library( Eos ${${projectName}_policy} ${Eos_src} )
set_property(TARGET Eos PROPERTY CXX_STANDARD 11)
set_property(TARGET Eos PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Eos PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( Eos DataBase)
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting Eos subdirectory")
endif(${verbose_traversal})
