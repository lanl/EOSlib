if(${verbose_traversal})
  message(STATUS "Entered MieGruneisen subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB MieGruneisen_h "*.h")
list( APPEND HEADER_FILES ${MieGruneisen_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB MieGruneisen_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${MieGruneisen_h})
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
file(GLOB MieGruneisen_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( MieGruneisen ${${projectName}_policy} ${MieGruneisen_src} )
set_property(TARGET MieGruneisen PROPERTY CXX_STANDARD 11)
set_property(TARGET MieGruneisen PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET MieGruneisen PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(MieGruneisen EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting MieGruneisen subdirectory")
endif(${verbose_traversal})
