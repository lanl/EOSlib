if(${verbose_traversal})
  message(STATUS "Entered Subs subdirectory")
endif(${verbose_traversal})

file(GLOB JWL_src . *.C)

set(JWL_src "${JWL_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Subs subdirectory")
endif(${verbose_traversal})
