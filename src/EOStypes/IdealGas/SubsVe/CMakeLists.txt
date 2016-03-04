if(${verbose_traversal})
  message(STATUS "Entered SubsVe subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND IdealGas_src ${foo})

set(IdealGas_src "${IdealGas_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsVe subdirectory")
endif(${verbose_traversal})