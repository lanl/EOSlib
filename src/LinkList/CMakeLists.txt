if(${verbose_traversal})
  message(STATUS "Entered LinkList subdirectory")
endif(${verbose_traversal})

message(STATUS "LinkList\n")
#
# headers
#
file(GLOB LinkList_h "*.h")
list( APPEND HEADER_FILES ${LinkList_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB LinkList_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${LinkList_h})
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
file(GLOB LinkList_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")

add_library( LinkList-obj OBJECT  ${LinkList_src} )
set_property(TARGET LinkList-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property(TARGET LinkList-obj PROPERTY CXX_STANDARD 11)
set_property(TARGET LinkList-obj PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET LinkList-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( LinkList ${${projectName}_policy} $<TARGET_OBJECTS:LinkList-obj> )
set_property(TARGET LinkList PROPERTY CXX_STANDARD 11)
set_property(TARGET LinkList PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET LinkList PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( LinkList Io )

#
# test
#
add_subdirectory(Tests) 


if(${verbose_traversal})
  message(STATUS "Exiting LinkList subdirectory")
endif(${verbose_traversal})
