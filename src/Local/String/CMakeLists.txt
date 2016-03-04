if(${verbose_traversal})
  message(STATUS "Entered String subdirectory")
endif(${verbose_traversal})

file(GLOB String_src . *.C)

set(String_src "${String_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting String subdirectory")
endif(${verbose_traversal})
