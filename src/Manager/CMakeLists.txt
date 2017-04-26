if(${verbose_traversal})
  message(STATUS "Entered Manager subdirectory")
endif(${verbose_traversal})

message(STATUS "Manager")
#
# headers
#
file(GLOB Manager_h "*.h")
list( APPEND HEADER_FILES ${Manager_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB Manager_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${Manager_h})
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
file(GLOB Manager_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
add_library( Manager-obj OBJECT ${Manager_src} )
  set_property(TARGET Manager-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET Manager-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET Manager-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET Manager-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( Manager ${${projectName}_policy}
                   $<TARGET_OBJECTS:Manager-obj>
                   $<TARGET_OBJECTS:ExtEOS-obj>
                   $<TARGET_OBJECTS:EOS-obj>
                   $<TARGET_OBJECTS:DataBase-obj>
                   $<TARGET_OBJECTS:CClib-obj> )                   
  set_property(TARGET Manager PROPERTY CXX_STANDARD 11)
  set_property(TARGET Manager PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET Manager PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
  target_link_libraries( Manager ExtEOS )
#
# Utilities
#
set( utils TestXmanager TestXRiemann TestXisentrope )
foreach(util ${utils})
   add_executable(${util} ${util}.C)
   target_link_libraries(${util} Manager)
endforeach(util)
#
# test
#
add_subdirectory(Tests)

if(${verbose_traversal})
  message(STATUS "Exiting Manager subdirectory")
endif(${verbose_traversal})
