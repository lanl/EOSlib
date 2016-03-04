if(${verbose_traversal})
  message(STATUS "Entered ODE subdirectory")
endif(${verbose_traversal})

set(ODE_src)

add_subdirectory(Subs)

add_library( ODE ${${projectName}_policy} ${ODE_src} )
set_property(TARGET ODE PROPERTY CXX_STANDARD 11)
set_property(TARGET ODE PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ODE PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( ODE Io Math OneDFunction LinkList)

list( APPEND tests Harmonic HarmonicF HarmonicB RandomHarmonic)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} ODE)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting ODE subdirectory")
endif(${verbose_traversal})