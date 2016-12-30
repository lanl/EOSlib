if(${verbose_traversal})
  message(STATUS "Entered TEST subdirectory")
endif(${verbose_traversal})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/compare_outputs.cmake
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#add_subdirectory(blessed_output)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(EOSLIB_BLESSED_OUTPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting TEST subdirectory")
endif(${verbose_traversal})
