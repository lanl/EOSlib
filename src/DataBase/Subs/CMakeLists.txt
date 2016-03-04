if(${verbose_traversal})
  message(STATUS "Entered Subs subdirectory")
endif(${verbose_traversal})

file(GLOB DataBase_src . *.C)

set(DataBase_src "${DataBase_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Subs subdirectory")
endif(${verbose_traversal})