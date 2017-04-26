if(${verbose_traversal})
  message(STATUS "Entered DataBase subdirectory")
endif(${verbose_traversal})

message(STATUS "DataBase\n")
#
# headers
#
file(GLOB DataBase_h "*.h")
list( APPEND HEADER_FILES ${DataBase_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB DataBase_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${DataBase_h})
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
file(GLOB DataBase_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")
add_library( DataBase-obj OBJECT ${DataBase_src} )
  set_property(TARGET DataBase-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET DataBase-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET DataBase-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET DataBase-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( DataBase ${${projectName}_policy}
                   $<TARGET_OBJECTS:DataBase-obj>
                   $<TARGET_OBJECTS:CClib-obj> )                   
  set_property(TARGET DataBase PROPERTY CXX_STANDARD 11)
  set_property(TARGET DataBase PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET DataBase PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries( DataBase ${CMAKE_DL_LIBS} )
#
# Utilities
#
file(GLOB util_c "*.C")

foreach( src_c ${util_c} )
  get_filename_component( util ${src_c} NAME_WE )
  add_executable(${util} ${src_c})
  target_link_libraries( ${util} DataBase )
endforeach( src_c )
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting DataBase subdirectory")
endif(${verbose_traversal})
