if(${verbose_traversal})
  message(STATUS "Entered SubsVT subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND Hayes_src ${foo})

set(Hayes_src "${Hayes_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsVT subdirectory")
endif(${verbose_traversal})
