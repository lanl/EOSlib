if(${verbose_traversal})
  message(STATUS "Entered IdealGas subdirectory")
endif(${verbose_traversal})

set(IdealGas_src)

add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( IdealGas ${${projectName}_policy} ${IdealGas_src} )
set_property(TARGET IdealGas PROPERTY CXX_STANDARD 11)
set_property(TARGET IdealGas PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET IdealGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( IdealGas Eos DataBase ODE)

list( APPEND tests Test2VT TestDetonation TestDeflagration TestPolar)

add_executable(TestIdealGas IdealGas.C)
target_link_libraries(TestIdealGas IdealGas)

add_executable(TestIdealGas1 Test1.C)
target_link_libraries(TestIdealGas1 IdealGas)

add_executable(TestIdealGas2 Test2.C)
target_link_libraries(TestIdealGas2 IdealGas)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} IdealGas)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting IdealGas subdirectory")
endif(${verbose_traversal})