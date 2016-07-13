if(${verbose_traversal})
  message(STATUS "Entered Subs subdirectory")
endif(${verbose_traversal})

file(GLOB ExtEOS_src . *.C)

set(ExtEOS_src "${ExtEOS_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Subs subdirectory")
endif(${verbose_traversal})
