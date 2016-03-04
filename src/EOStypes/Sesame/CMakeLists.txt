if(${verbose_traversal})
  message(STATUS "Entered Sesame subdirectory")
endif(${verbose_traversal})

set(Sesame_src)

add_subdirectory(Subs)

add_library( Sesame ${${projectName}_policy} ${Sesame_src} )
set_property(TARGET Sesame PROPERTY CXX_STANDARD 11)
set_property(TARGET Sesame PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Sesame PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Sesame Eos DataBase ODE )

set( tests )

#add_executable(TestSesame Sesame.C)
#target_link_libraries(TestSesame Sesame)

add_executable(TestSesame1 Test2.C)
target_link_libraries(TestSesame1 Sesame)

add_executable(TestSesame2 Test2VT.C)
target_link_libraries(TestSesame2 Sesame)

add_executable(TestSesame3 Test3.C)
target_link_libraries(TestSesame3 Sesame)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Sesame)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Sesame subdirectory")
endif(${verbose_traversal})