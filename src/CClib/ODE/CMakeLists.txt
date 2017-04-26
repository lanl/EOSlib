if(${verbose_traversal})
  message(STATUS "Entered ODE subdirectory")
endif(${verbose_traversal})

message(STATUS "ODE\n")
#
# headers
#
file(GLOB ODE_h "*.h")
list( APPEND HEADER_FILES ${ODE_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB ODE_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${ODE_h})
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
file(GLOB ODE_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")

add_library( ODE-obj OBJECT  ${ODE_src} )
set_property(TARGET ODE-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property(TARGET ODE-obj PROPERTY CXX_STANDARD 11)
set_property(TARGET ODE-obj PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ODE-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( ODE ${${projectName}_policy} $<TARGET_OBJECTS:ODE-obj> )
set_property(TARGET ODE PROPERTY CXX_STANDARD 11)
set_property(TARGET ODE PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ODE PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( ODE Math LinkList OneDfunction )
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting ODE subdirectory")
endif(${verbose_traversal})
