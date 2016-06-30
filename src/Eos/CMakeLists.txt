if(${verbose_traversal})
  message(STATUS "Entered Eos subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in Eos: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_EOS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Eos)

set(Eos_src)

add_subdirectory(SubsBase)
add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( Eos ${${projectName}_policy} ${Eos_src} )
set_property(TARGET Eos PROPERTY CXX_STANDARD 11)
set_property(TARGET Eos PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Eos PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Eos Spline ODE Calc DataBase)

list( APPEND tests Test1 Test2 Test3 )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Eos)
#  add_test(${test} ${test})
endforeach(test)

#add_executable(Test3 Test3.C)
#target_link_libraries(Test3 Eos)
#add_test(NAME Test3 COMMAND Test3 file ${EOSLIB_DATA_PATH}/test_data/EosTest.data )
#set_tests_properties(Test3 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

#set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data type hydro name std")
add_test(NAME Test1
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Test1
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_EOS_BLESSED_OUTPUT}/Test1Out.blessed
  -D err_blessed=${EOSLIB_EOS_BLESSED_OUTPUT}/Test1Err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
set_tests_properties(Test1 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

#Produces same error as blessed output but includes different path info before test name
# #set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base Units type hydro name std use")
# add_test(NAME Test2
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Test2
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/Test2Out.blessed
#   -D err_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/Test2Err.blessed 
#   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#   )
# set_tests_properties(Test2 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
 
#Produces same error as blessed output but includes different path info before test name
# #set(args "file ${EOSLIB_DATA_PATH}/test_data/DataBaseUnits.data base ''")	# global base class")
# add_test(NAME Test3
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/Test3
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/Test3Out.blessed
#   -D err_blessed=${EOSLIB_DATABASE_BLESSED_OUTPUT}/Test3Err.blessed 
#   -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#   )
# set_tests_properties(Test3 PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")

if(${verbose_traversal})
  message(STATUS "Exiting Eos subdirectory")
endif(${verbose_traversal})
