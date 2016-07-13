if(${verbose_traversal})
  message(STATUS "Entered Subs subdirectory")
endif(${verbose_traversal})

file(GLOB Manager_src . *.C)

set(Manager_src "${Manager_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Subs subdirectory")
endif(${verbose_traversal})
