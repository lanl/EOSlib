if(${verbose_traversal})
  message(STATUS "Entered Hayes subdirectory")
endif(${verbose_traversal})

set(Hayes_src)

add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( Hayes ${${projectName}_policy} ${Hayes_src} )
set_property(TARGET Hayes PROPERTY CXX_STANDARD 11)
set_property(TARGET Hayes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Hayes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Hayes Eos DataBase ODE)

list( APPEND tests Test1 Test2 Test2VT )

# Many of these tests output to the terminal. This output should be checked, but isn't.

add_executable(Test1Hayes Test1.C)
target_link_libraries(Test1Hayes Hayes)
add_test(Test1Hayes Test1Hayes)


foreach(test Test2 Test2VT)
  add_executable(${test}Hayes ${test}.C)
  target_link_libraries(${test}Hayes Hayes)
  add_test(NAME ${test}Hayes COMMAND ${test}Hayes
    file ${EOSLIB_DATA_PATH}/test_data/HayesTest.data)
  set_tests_properties(${test}Hayes PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

list(APPEND INSTALL_TARGETS Hayes)
set(INSTALL_TARGETS PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Hayes subdirectory")
endif(${verbose_traversal})
