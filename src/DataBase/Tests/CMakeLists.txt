if(${verbose_traversal})
  message(STATUS "Entered DataBase/Tests subdirectory")
endif(${verbose_traversal})

set(BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/DataBase)
#
#   Test Utilities
#
set( bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
#
#
set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use")
add_test(NAME Data0
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data0out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data0err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(Data0 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data")
add_test(NAME Data1
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data1out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data1err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(Data1 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base EOS")
add_test(NAME Data2
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data2out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data2err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(Data2 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

# prints absolute path
set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data type DataBase name Environment parameters")
# add_test(NAME Data3
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${bin}/Data
#   -D test_args:string=${args}
#   -D out_blessed=${BLESSED_OUTPUT}/data3out.blessed
#   -D err_blessed=${BLESSED_OUTPUT}/data3err.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
# set_tests_properties(Data3 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")


set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data type SharedLibs parameters")
add_test(NAME Data4
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data4out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data4err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
set_tests_properties(Data4 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type UnitsNames name std parameters")
add_test(NAME Data6
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data6out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data6err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type UnitsNames name std use")
add_test(NAME Data7
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data7out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data7err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base Units type hydro name std use")
add_test(NAME Data8
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data8out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data8err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base EOS type Hayes name HMX")
add_test(NAME Data9
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data9out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data9err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std parameters")
add_test(NAME Data10
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data10out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data10err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use")
add_test(NAME Data11
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data11out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data11err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseTest.data base EOS type Hayes name HMX-YC parameters")
add_test(NAME Data12
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Data
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/data12out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/data12err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )

set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro from std to HiP e u u^2 P V P*V")
add_test(NAME Convert0
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${bin}/Convert
  -D test_args:string=${args}
  -D out_blessed=${BLESSED_OUTPUT}/convert0out.blessed
  -D err_blessed=${BLESSED_OUTPUT}/convert0err.blessed 
  -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
  )
#
# test of dynamic linking
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
#
#
file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Test*.C")
foreach( test_c ${Test_c} )
  get_filename_component( test ${test_c} NAME_WE )
  add_executable(${test} ${test_c})
  target_link_libraries( ${test} DataBase )
endforeach( test_c )
#
#
add_library( shared_obj ${${projectName}_policy} shared_obj.C)
target_link_libraries( shared_obj Math )
#
# add tests
set( bin "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )


if(${verbose_traversal})
  message(STATUS "Exiting DataBase/Tests subdirectory")
endif(${verbose_traversal})

