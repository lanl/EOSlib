if(${verbose_traversal})
  message(STATUS "Entered SubBase subdirectory")
endif(${verbose_traversal})

file(GLOB foo . *.C)
list(APPEND Eos_src ${foo})

set(Eos_src "${Eos_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting SubBase subdirectory")
endif(${verbose_traversal})