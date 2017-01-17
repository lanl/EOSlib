if(${verbose_traversal})
  message(STATUS "Entered Calc subdirectory")
endif(${verbose_traversal})

message(STATUS "Calc\n")
#
# headers
#
file(GLOB Calc_h "*.h")
list( APPEND HEADER_FILES ${Calc_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )
<<<<<<< HEAD

file(GLOB Calc_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Calc_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)
#
# SOURCE
#
=======

file(GLOB Calc_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Calc_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)

>>>>>>> c30ac671e07def170a16c86a2343d793370d798c
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
file(GLOB Calc_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")

add_library( Calc-obj OBJECT  ${Calc_src} )
set_property(TARGET Calc-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property(TARGET Calc-obj PROPERTY CXX_STANDARD 11)
set_property(TARGET Calc-obj PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Calc-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( Calc ${${projectName}_policy} $<TARGET_OBJECTS:Calc-obj> )
set_property(TARGET Calc PROPERTY CXX_STANDARD 11)
set_property(TARGET Calc PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( Calc Io Math LinkList )
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting Calc subdirectory")
endif(${verbose_traversal})
