if(${verbose_traversal})
  message(STATUS "Entered ElasticPlastic subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB ElasticPlastic_h "*.h")
list( APPEND HEADER_FILES ${ElasticPlastic_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB ElasticPlastic_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${ElasticPlastic_h})
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
file(GLOB ElasticPlastic_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( ElasticPlastic-obj OBJECT ${ElasticPlastic_src} )
  set_property(TARGET ElasticPlastic-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET ElasticPlastic-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET ElasticPlastic-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ElasticPlastic-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( ElasticPlastic ${${projectName}_policy}
                 $<TARGET_OBJECTS:ElasticPlastic-obj> )
  set_property(TARGET ElasticPlastic PROPERTY CXX_STANDARD 11)
  set_property(TARGET ElasticPlastic PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ElasticPlastic PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries(ElasticPlastic ExtEOS)
#
# Tests
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting ElasticPlastic subdirectory")
endif(${verbose_traversal})
