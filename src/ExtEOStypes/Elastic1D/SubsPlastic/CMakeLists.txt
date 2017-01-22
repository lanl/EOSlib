if(${verbose_traversal})
  message(STATUS "Entered SubsPlastic subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND Elastic1D_src ${foo})

set(Elastic1D_src "${Elastic1D_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsPlastic subdirectory")
endif(${verbose_traversal})
