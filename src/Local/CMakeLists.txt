if(${verbose_traversal})
  message(STATUS "Entered Local subdirectory")
endif(${verbose_traversal})

set(Io_src)
add_subdirectory(Io)

set(Math_src)
add_subdirectory(Math)

set(String_src)
add_subdirectory(String)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
 
add_library( Io ${${projectName}_policy} ${Io_src} )
set_property(TARGET Io PROPERTY CXX_STANDARD 11)
set_property(TARGET Io PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Io PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( Math ${${projectName}_policy} ${Math_src} )
set_property(TARGET Math PROPERTY CXX_STANDARD 11)
set_property(TARGET Math PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Math PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( String ${${projectName}_policy} ${String_src} )
set_property(TARGET String PROPERTY CXX_STANDARD 11)
set_property(TARGET String PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET String PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

if(${verbose_traversal})
  message(STATUS "Exiting Local subdirectory")
endif(${verbose_traversal})
