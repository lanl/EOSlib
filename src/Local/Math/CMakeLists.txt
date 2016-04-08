if(${verbose_traversal})
  message(STATUS "Entered Math subdirectory")
endif(${verbose_traversal})

file(GLOB Math_src . *.C)

set(Math_src "${Math_src}" PARENT_SCOPE)

add_subdirectory(Test)

if(${verbose_traversal})
  message(STATUS "Exiting Math subdirectory")
endif(${verbose_traversal})
