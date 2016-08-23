if(${verbose_traversal})
  message(STATUS "Entered Calc subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in Calc: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_CALC_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Calc)
set(Calc_src)

add_subdirectory(Subs)

add_library( Calc ${${projectName}_policy} ${Calc_src} )
set_property(TARGET Calc PROPERTY CXX_STANDARD 11)
set_property(TARGET Calc PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Calc LinkList Io Math String)

list( APPEND tests TestCalc)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Calc)
  # TestCalc requires interactive input.
  
  set(args "echo -e 'm(2,2)\nrho\nabc'")##this line provides correct output from terminal
  set(args "rho abc m(2,2)")

  add_test(NAME ${test}
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_CALC_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_CALC_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS Applications ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

list(APPEND INSTALL_TARGETS Calc)
set(INSTALL_TARGETS PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Calc subdirectory")
endif(${verbose_traversal})
