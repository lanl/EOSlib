if(${verbose_traversal})
  message(STATUS "Entered EOStypes subdirectory")
endif(${verbose_traversal})

set(EOSLIB_EOSTYPES_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes)

list(APPEND EOStypes_libs IdealGas StiffenedGas Hayes HayesBM GenHayes MieGruneisen Davis JWL Porous PTequilibrium )
foreach(EOStype ${EOStypes_libs})
  add_subdirectory(${EOStype})
endforeach(EOStype)

add_library( Materials ${${projectName}_policy} dummy.cpp)
set_property(TARGET Materials PROPERTY CXX_STANDARD 11)
set_property(TARGET Materials PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Materials PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
target_link_libraries( Materials ${EOStypes_libs})

# add_library( MaterialsLib ${${projectName}_policy} dummy.cpp)
# set_property(TARGET MaterialsLib PROPERTY CXX_STANDARD 11)
# set_property(TARGET MaterialsLib PROPERTY CXX_STANDARD_REQUIRED ON)
# target_link_libraries( MaterialsLib Materials )

list( APPEND tests TestRiemann )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Materials)
  # add_test(TestRiemann TestRiemann)
  add_test(NAME TestRiemann
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/TestRiemann
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_EOSTYPES_BLESSED_OUTPUT}/TestRiemannOut.blessed
    -D err_blessed=${EOSLIB_SESAME_BLESSED_OUTPUT}/TestRiemannErr.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
   )
 set_tests_properties(TestRiemann PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

list(APPEND INSTALL_TARGETS Materials)
set(INSTALL_TARGETS PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting EOStypes subdirectory")
endif(${verbose_traversal})
 
