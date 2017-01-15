if(${verbose_traversal})
  message(STATUS "Entered PTequilibrium subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB PTequilibrium_h "*.h")
list( APPEND HEADER_FILES ${PTequilibrium_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB PTequilibrium_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${PTequilibrium_h})
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
file(GLOB PTequilibrium_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( PTequilibrium ${${projectName}_policy} ${PTequilibrium_src} )
set_property(TARGET PTequilibrium PROPERTY CXX_STANDARD 11)
set_property(TARGET PTequilibrium PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET PTequilibrium PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries(PTequilibrium Eos)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting PTequilibrium subdirectory")
endif(${verbose_traversal})
