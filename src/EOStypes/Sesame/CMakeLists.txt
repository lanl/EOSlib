if(${verbose_traversal})
  message(STATUS "Entered Sesame subdirectory")
endif(${verbose_traversal})

set(EOSLIB_SESAME_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/Sesame)

set(Sesame_src)

add_subdirectory(Subs)

add_library( Sesame ${${projectName}_policy} ${Sesame_src} )
set_property(TARGET Sesame PROPERTY CXX_STANDARD 11)
set_property(TARGET Sesame PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Sesame PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Sesame Eos DataBase ODE )

list( APPEND tests Test2 Test2VT Test3)

#add_executable(TestSesame Sesame.C)
#target_link_libraries(TestSesame Sesame)

# add_executable(TestSesame1 Test2.C)
# target_link_libraries(TestSesame1 Sesame)

# add_executable(TestSesame2 Test2VT.C)
# target_link_libraries(TestSesame2 Sesame)

# add_executable(TestSesame3 Test3.C)
# target_link_libraries(TestSesame3 Sesame
#  )

foreach(test ${tests})
  add_executable(${test}Sesame ${test}.C)
  target_link_libraries(${test}Sesame Sesame)
  add_test(NAME ${test}Sesame
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Sesame
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_SESAME_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_SESAME_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
   )
 set_tests_properties(${test}Sesame PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Sesame subdirectory")
endif(${verbose_traversal})
