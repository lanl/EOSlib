if(${verbose_traversal})
  message(STATUS "Entered DnKappa/Tests subdirectory")
endif(${verbose_traversal})

set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/DnKappa)
set( bin ${CMAKE_CURRENT_BINARY_DIR} )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${bin}")

list( APPEND tests TestDnKappa )


foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} DnKappa)
#  add_test(NAME ${test}
#  COMMAND ${CMAKE_COMMAND}
#  -D test_cmd=${bin}/${test}
#  -D test_args:string=${args}
#  -D out_blessed=${BLESSED_OUTPUT}/${test}Out.blessed
#  -D err_blessed=${BLESSED_OUTPUT}/${test}Err.blessed 
#  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#  )
#  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)


if(${verbose_traversal})
  message(STATUS "Exiting DnKappa/Tests subdirectory")
endif(${verbose_traversal})
