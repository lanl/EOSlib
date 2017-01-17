if(${verbose_traversal})
  message(STATUS "Entered Local subdirectory")
endif(${verbose_traversal})

message(STATUS "Local\n")
#
# headers
#
file(GLOB Local_h "*.h")
list( APPEND HEADER_FILES ${Local_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Local_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Local_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)
<<<<<<< HEAD
#
# SOURCE
#
=======

>>>>>>> c30ac671e07def170a16c86a2343d793370d798c
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
list( APPEND subs Io Math String )
foreach(sub ${subs})
  file(GLOB src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${sub}/*.C")
 
  add_library( ${sub}-obj OBJECT  ${src} )
  set_property(TARGET ${sub}-obj PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_property(TARGET ${sub}-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET ${sub}-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ${sub}-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  add_library( ${sub} ${${projectName}_policy} $<TARGET_OBJECTS:${sub}-obj> )
 
  if( ${BoostTest} )
      add_subdirectory(${sub}/Test)
  endif( ${BoostTest} )
endforeach(sub)
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting Local subdirectory")
endif(${verbose_traversal})
