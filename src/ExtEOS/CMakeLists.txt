if(${verbose_traversal})
  message(STATUS "Entered ExtEOS subdirectory")
endif(${verbose_traversal})

message(STATUS "ExtEOS")
#
# headers
#
file(GLOB ExtEOS_h "*.h")
list( APPEND HEADER_FILES ${ExtEOS_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB ExtEOS_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${ExtEOS_h})
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
file(GLOB ExtEOS_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
add_library( ExtEOS-obj OBJECT ${ExtEOS_src} )
  set_property(TARGET ExtEOS-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET ExtEOS-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET ExtEOS-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ExtEOS-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( ExtEOS ${${projectName}_policy}
                   $<TARGET_OBJECTS:ExtEOS-obj>
                   $<TARGET_OBJECTS:EOS-obj>
                   $<TARGET_OBJECTS:DataBase-obj>
                   $<TARGET_OBJECTS:CClib-obj> )                   
  set_property(TARGET ExtEOS PROPERTY CXX_STANDARD 11)
  set_property(TARGET ExtEOS PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ExtEOS PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries( ExtEOS ${CMAKE_DL_LIBS} )
#
# test
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting ExtEOS subdirectory")
endif(${verbose_traversal})

