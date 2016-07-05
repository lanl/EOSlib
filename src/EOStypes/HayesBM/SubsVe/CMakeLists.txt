if(${verbose_traversal})
  message(STATUS "Entered SubsVe subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND HayesBM_src ${foo})

set(HayesBM_src "${HayesBM_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsVe subdirectory")
endif(${verbose_traversal})
