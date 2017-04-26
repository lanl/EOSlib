if(${verbose_traversal})
  message(STATUS "Entered IdealGas subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB IdealGas_h "*.h")
list( APPEND HEADER_FILES ${IdealGas_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB IdealGas_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${IdealGas_h})
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
file(GLOB IdealGas_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( IdealGas-obj OBJECT ${IdealGas_src} )
  set_property(TARGET IdealGas-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET IdealGas-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET IdealGas-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET IdealGas-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( IdealGas ${${projectName}_policy}
                   $<TARGET_OBJECTS:IdealGas-obj> )
  set_property(TARGET IdealGas PROPERTY CXX_STANDARD 11)
  set_property(TARGET IdealGas PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET IdealGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries(IdealGas EOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting IdealGas subdirectory")
endif(${verbose_traversal})
