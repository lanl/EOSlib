if(${verbose_traversal})
  message(STATUS "Entered EOStypes subdirectory")
endif(${verbose_traversal})

set(EOStypes_src)

# add_subdirectory(Davis)
# add_subdirectory(GenHayes)
 add_subdirectory(Hayes)
# add_subdirectory(HayesBM)
add_subdirectory(IdealGas)
# add_subdirectory(JWL)
# add_subdirectory(MieGruneisen)
# add_subdirectory(PTequilibrium)
# add_subdirectory(Porous)
#add_subdirectory(Sesame)
 add_subdirectory(StiffenedGas)
# add_subdirectory(Uniaxial)

add_library( EOStypes ${${projectName}_policy} dummy.cpp)
set_property(TARGET EOStypes PROPERTY CXX_STANDARD 11)
set_property(TARGET EOStypes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET EOStypes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( EOStypes IdealGas )

list( APPEND tests TestRiemann )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} EOStypes)
  add_test(TestRiemann TestRiemann)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting EOStypes subdirectory")
endif(${verbose_traversal})
