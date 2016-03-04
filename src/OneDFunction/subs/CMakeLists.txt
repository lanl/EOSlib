if(${verbose_traversal})
  message(STATUS "Entered Subs subdirectory")
endif(${verbose_traversal})

file(GLOB OneDFunction_src . *.C)

set(OneDFunction_src "${OneDFunction_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Subs subdirectory")
endif(${verbose_traversal})