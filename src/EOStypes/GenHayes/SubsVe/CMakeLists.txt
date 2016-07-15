if(${verbose_traversal})
  message(STATUS "Entered SubsVe subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND GenHayes_src ${foo})

set(GenHayes_src "${GenHayes_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsVe subdirectory")
endif(${verbose_traversal})
