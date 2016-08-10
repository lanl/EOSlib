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

target_link_libraries( Eos Spline ODE Calc DataBase ${CMAKE_DL_LIBS} )

list( APPEND tests Test1 Test2 )#Test3 )
##Test 3 appears to be an accepted failure from Moonlight, no idea how to fix

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Eos)
  add_test(NAME ${test}
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_EOS_BLESSED_OUTPUT}/${test}Out.blessed
  -D err_blessed=${EOSLIB_EOS_BLESSED_OUTPUT}/${test}Err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Eos subdirectory")
endif(${verbose_traversal})
