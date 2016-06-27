if(${verbose_traversal})
  message(STATUS "Entered TEST subdirectory")
endif(${verbose_traversal})

add_subdirectory(blessed_output)

set(EOSLIB_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting TEST subdirectory")
endif(${verbose_traversal})
