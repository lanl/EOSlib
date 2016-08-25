if(${verbose_traversal})
  message(STATUS "Entered subs subdirectory")
endif(${verbose_traversal})

file(GLOB LinkList_src . *.C)

set(LinkList_src "${LinkList_src}" PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting subs subdirectory")
endif(${verbose_traversal})