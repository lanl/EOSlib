if(${verbose_traversal})
  message(STATUS "Entered SubsVT subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND HayesBM_src ${foo})

set(HayesBM_src "${HayesBM_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsVT subdirectory")
endif(${verbose_traversal})
