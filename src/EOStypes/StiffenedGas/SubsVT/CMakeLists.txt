if(${verbose_traversal})
  message(STATUS "Entered SubsVT subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND StiffenedGas_src ${foo})

set(StiffenedGas_src "${StiffenedGas_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsVT subdirectory")
endif(${verbose_traversal})
