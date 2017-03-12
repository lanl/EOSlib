if(${verbose_traversal})
  message(STATUS "Entered EOS subdirectory")
endif(${verbose_traversal})

message(STATUS "EOS")
#
# headers
#
file(GLOB EOS_h "*.h")
list( APPEND HEADER_FILES ${EOS_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB EOS_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${EOS_h})
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
file(GLOB EOS_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
add_library( EOS-obj OBJECT ${EOS_src} )
  set_property(TARGET EOS-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET EOS-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET EOS-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET EOS-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( EOS ${${projectName}_policy}
                   $<TARGET_OBJECTS:EOS-obj>
                   $<TARGET_OBJECTS:DataBase-obj>
                   $<TARGET_OBJECTS:CClib-obj> )                   
  set_property(TARGET EOS PROPERTY CXX_STANDARD 11)
  set_property(TARGET EOS PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET EOS PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries( EOS ${CMAKE_DL_LIBS} )
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting EOS subdirectory")
endif(${verbose_traversal})
