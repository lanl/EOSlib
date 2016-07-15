if(${verbose_traversal})
  message(STATUS "Entered SubsPhi subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND Porous_src ${foo})

set(Porous_src "${Porous_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubsPhi subdirectory")
endif(${verbose_traversal})
