if(${verbose_traversal})
  message(STATUS "Entered DataBase subdirectory")
endif(${verbose_traversal})

set(DataBase_src)

add_subdirectory(Subs)
add_subdirectory(blessed_output)

add_library( DataBase ${${projectName}_policy} ${DataBase_src} )
set_property(TARGET DataBase PROPERTY CXX_STANDARD 11)
set_property(TARGET DataBase PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DataBase PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( shared_obj ${${projectName}_policy} shared_obj.C)
target_link_libraries( shared_obj Math )

target_link_libraries( DataBase Calc)
target_link_libraries( DataBase ${CMAKE_DL_LIBS})

list( APPEND tests Data Convert TestDB Testdl )

#configure_file(Test.data ${CMAKE_CURRENT_BINARY_DIR}/Test.data COPYONLY)
#configure_file(Units.data ${CMAKE_CURRENT_BINARY_DIR}/Units.data COPYONLY)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} DataBase)
#  add_test(${test} ${test})
endforeach(test)

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME TestDB
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/TestDB
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/testdbout.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/testdberr.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
set_tests_properties(TestDB PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use")
add_test(NAME Data0
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data0out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data0err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

#These tests aren't actually doing anything; they need to compare terminal
# output in order to yield a pass/fail. The test commands are included for
# future use.
#add_test(NAME Data0 COMMAND Data file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use)

target_link_libraries(Testdl shared_obj)

if(${verbose_traversal})
  message(STATUS "Exiting DataBase subdirectory")
endif(${verbose_traversal})
