if(${verbose_traversal})
  message(STATUS "Entered Io subdirectory")
endif(${verbose_traversal})

file(GLOB Io_src . *.C)

set(Io_src "${Io_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Io subdirectory")
endif(${verbose_traversal})
