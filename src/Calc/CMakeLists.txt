if(${verbose_traversal})
  message(STATUS "Entered Calc subdirectory")
endif(${verbose_traversal})

set(Calc_src)

add_subdirectory(Subs)

include_directories(.)

# add_library( Calc ${${projectName}_policy} ${Calc_src} )
# set_property(TARGET Calc PROPERTY CXX_STANDARD 11)
# set_property(TARGET Calc PROPERTY CXX_STANDARD_REQUIRED ON)
# set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

if(${verbose_traversal})
  message(STATUS "Exiting Calc subdirectory")
endif(${verbose_traversal})
