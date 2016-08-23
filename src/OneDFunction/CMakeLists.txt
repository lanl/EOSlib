if(${verbose_traversal})
  message(STATUS "Entered OneDFunction subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in database: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_DATABASE_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/OneDFunction)

set(OneDFunction_src)

add_subdirectory(subs)
#add_subdirectory(blessed_output)

add_library( OneDFunction ${${projectName}_policy} ${OneDFunction_src} )
set_property(TARGET OneDFunction PROPERTY CXX_STANDARD 11)
set_property(TARGET OneDFunction PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET OneDFunction PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( OneDFunction Io Math)

list( APPEND tests Test Xtest TestI)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} OneDFunction)
#  add_test(${test} ${test})
endforeach(test)

#What is this line doing?
#message(STATUS "database blessed output ${EOSLIB_DATABASE_BLESSED_OUTPUT}/testdbout.blessed\n")

#set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME Test
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Test
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/TestOut.blessed
  -D err_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/TestErr.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
set_tests_properties(Test PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

#set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME Xtest
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Xtest
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/XtestOut.blessed
  -D err_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/XtestErr.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
set_tests_properties(Xtest PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

#set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME TestI
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/TestI
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/TestIOut.blessed
  -D err_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/TestIErr.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
set_tests_properties(TestI PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

list(APPEND INSTALL_TARGETS OneDFunction)
set(INSTALL_TARGETS PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting OneDFunction subdirectory")
endif(${verbose_traversal})

