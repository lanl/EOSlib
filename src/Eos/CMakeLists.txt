if(${verbose_traversal})
  message(STATUS "Entered Eos subdirectory")
endif(${verbose_traversal})

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
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Eos subdirectory")
endif(${verbose_traversal})
