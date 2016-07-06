if(${verbose_traversal})
  message(STATUS "Entered JWL subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in JWL: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_JWL_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/JWL)
set(JWL_src)

add_subdirectory(Subs)

add_library( JWL ${${projectName}_policy} ${JWL_src} )
set_property(TARGET JWL PROPERTY CXX_STANDARD 11)
set_property(TARGET JWL PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET JWL PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( JWL Eos DataBase ODE Calc ${CMAKE_DL_LIBS})

list (APPEND tests Test1JWL_VT Test2JWL_VT TestBKW TestDeflagration TestDetonation TestHEfit TestHEprods TestJWL TestJWLPT)

foreach(test ${tests} )
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} JWL)
  set(args "file ${EOSLIB_DATA_PATH}/test_data/JWLTest.data")
  add_test(NAME ${test}
  COMMAND ${CMAKE_COMMAND}
  -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
  -D test_args:string=${args}
  -D out_blessed=${EOSLIB_JWL_BLESSED_OUTPUT}/${test}Out.blessed
  -D err_blessed=${EOSLIB_JWL_BLESSED_OUTPUT}/${test}Err.blessed 
  -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
  )
  set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting JWL subdirectory")
endif(${verbose_traversal})
