if(${verbose_traversal})
  message(STATUS "Entered EOStypes subdirectory")
endif(${verbose_traversal})

set(EOStypes_src)
set(EOSLIB_EOSTYPES_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes)
add_subdirectory(Davis)
list(APPEND EOStypes_libs IdealGas StiffenedGas Hayes HayesBM GenHayes MieGruneisen Davis JWL PTequilibrium)
# add_subdirectory(GenHayes)
add_subdirectory(Hayes)
add_subdirectory(HayesBM)
add_subdirectory(IdealGas)
add_subdirectory(JWL)
add_subdirectory(MieGruneisen)
add_subdirectory(PTequilibrium)
# add_subdirectory(Porous)
# add_subdirectory(Sesame)
add_subdirectory(StiffenedGas)
# add_subdirectory(Uniaxial)

add_library( EOStypes ${${projectName}_policy} dummy.cpp)
set_property(TARGET EOStypes PROPERTY CXX_STANDARD 11)
set_property(TARGET EOStypes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET EOStypes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( EOStypes Hayes HayesBM IdealGas JWL MieGruneisen PTequilibrium StiffenedGas )

list( APPEND tests TestRiemann )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} EOStypes)
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

if(${verbose_traversal})
  message(STATUS "Exiting EOStypes subdirectory")
endif(${verbose_traversal})
