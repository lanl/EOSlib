if(${verbose_traversal})
  message(STATUS "Entered Spline subdirectory")
endif(${verbose_traversal})


message(STATUS "Spline\n")
#
# headers
#
file(GLOB Spline_h "*.h")
list( APPEND HEADER_FILES ${Spline_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Spline_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Spline_h})
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
file(GLOB Spline_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")

add_library( Spline-obj OBJECT  ${Spline_src} )
set_property(TARGET Spline-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property(TARGET Spline-obj PROPERTY CXX_STANDARD 11)
set_property(TARGET Spline-obj PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Spline-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( Spline ${${projectName}_policy} $<TARGET_OBJECTS:Spline-obj> )
set_property(TARGET Spline PROPERTY CXX_STANDARD 11)
set_property(TARGET Spline PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Spline PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( Spline Io Math )
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting Spline subdirectory")
endif(${verbose_traversal})
