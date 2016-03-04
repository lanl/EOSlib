if(${verbose_traversal})
  message(STATUS "Entered OneDFunction subdirectory")
endif(${verbose_traversal})

set(OneDFunction_src)

add_subdirectory(subs)

add_library( OneDFunction ${${projectName}_policy} ${OneDFunction_src} )
set_property(TARGET OneDFunction PROPERTY CXX_STANDARD 11)
set_property(TARGET OneDFunction PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET OneDFunction PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( OneDFunction Io Math)

list( APPEND tests Test Xtest TestI)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} OneDFunction)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting OneDFunction subdirectory")
endif(${verbose_traversal})