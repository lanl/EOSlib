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

###startHere#######################################jrc2016

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base ''		# global base class")
add_test(NAME Data1
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data1out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data1err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base EOS")
add_test(NAME Data2
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data2out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data2err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data type DataBase name Environment parameters")
add_test(NAME Data3
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data3out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data3err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data type SharedLibs parameters")
add_test(NAME Data4
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data4out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data4err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

##$HEdata - not sure if this is defined correctly
set(args "file ${EOSLIB_DATA_PATH}/test_data/$HEdata type SharedLibs parameters")
add_test(NAME Data5
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data5out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data5err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data UnitsNames name std parameters")
add_test(NAME Data6
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data6out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data6err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type UnitsNames name std use")
add_test(NAME Data7
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data7out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data7err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base Units type hydro name std use")
add_test(NAME Data8
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data8out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data8err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base EOS type Hayes name HMX")
add_test(NAME Data9
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data9out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data9err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std parameters")
add_test(NAME Data10
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data10out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data10err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use")
add_test(NAME Data11
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data11out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data11err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base EOS type Hayes name HMX-YC parameters")
add_test(NAME Data12
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Data
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/data12out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/data12err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro from std to HiP e u u^2 P V 'P*V'")
add_test(NAME Convert0
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Convert
  -D test_args:string=${args}
  -D out_blessed=${CMAKE_CURRENT_BINARY_DIR}/convert0out.blessed
  -D err_blessed=${CMAKE_CURRENT_BINARY_DIR}/convert0err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
###endHere###############################delete me

#These tests aren't actually doing anything; they need to compare terminal
# output in order to yield a pass/fail. The test commands are included for
# future use.
#add_test(NAME Data0 COMMAND Data file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use)

target_link_libraries(Testdl shared_obj)

if(${verbose_traversal})
  message(STATUS "Exiting DataBase subdirectory")
endif(${verbose_traversal})
