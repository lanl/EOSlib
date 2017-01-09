if(${verbose_traversal})
  message(STATUS "Entered OneDFunction subdirectory")
endif(${verbose_traversal})


message(STATUS "OneDFunction\n")
#
# headers
#
file(GLOB OneDfunction_h "*.h")
list( APPEND HEADER_FILES ${OneDfunction_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB OneDfunction_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${OneDfunction_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)

execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
file(GLOB OneDfunction_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")

add_library( OneDfunction-obj OBJECT  ${OneDfunction_src} )
set_property(TARGET OneDfunction-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property(TARGET OneDfunction-obj PROPERTY CXX_STANDARD 11)
set_property(TARGET OneDfunction-obj PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET OneDfunction-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( OneDfunction ${${projectName}_policy} $<TARGET_OBJECTS:OneDfunction-obj> )
set_property(TARGET OneDfunction PROPERTY CXX_STANDARD 11)
set_property(TARGET OneDfunction PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET OneDfunction PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
#
# test
#
add_subdirectory(Tests)


if(${verbose_traversal})
  message(STATUS "Exiting OneDFunction subdirectory")
endif(${verbose_traversal})

