if(${verbose_traversal})
  message(STATUS "Entered DnKappa subdirectory")
endif(${verbose_traversal})

message(STATUS "DnKappa\n")
#
# headers
#
file(GLOB DnKappa_h "*.h")
list( APPEND HEADER_FILES ${DnKappa_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB DnKappa_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${DnKappa_h})
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
file(GLOB DnKappa_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
add_library( DnKappa ${${projectName}_policy} ${DnKappa_src} )
set_property(TARGET DnKappa PROPERTY CXX_STANDARD 11)
set_property(TARGET DnKappa PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DnKappa PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET DnKappa PROPERTY LINKER_LANGUAGE CXX)
target_link_libraries( DnKappa EOS)
#
# Utility
#
add_executable(Dn_kappa Dn_kappa.C)
target_link_libraries(Dn_kappa DnKappa)
#
# test
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting DnKappa subdirectory")
endif(${verbose_traversal})
