if(${verbose_traversal})
  message(STATUS "Entered OneDfunction/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.C")

foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} OneDfunction LinkList Io Math String )
endforeach( test_c )

set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/OneDFunction)
set( bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

add_test(NAME Test
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Test
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/TestOut.blessed
  -D err_blessed=${BLESSED_OUTPUT}/TestErr.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(Test PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

add_test(NAME Xtest
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Xtest
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/XtestOut.blessed
  -D err_blessed=${BLESSED_OUTPUT}/XtestErr.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(Xtest PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

#set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME TestI
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/TestI
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/TestIOut.blessed
  -D err_blessed=${BLESSED_OUTPUT}/TestIErr.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(TestI PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")



if(${verbose_traversal})
  message(STATUS "Exiting OneDfunction/Tests subdirectory")
endif(${verbose_traversal})
