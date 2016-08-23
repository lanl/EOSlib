if(${verbose_traversal})
  message(STATUS "Entered LinkList subdirectory")
endif(${verbose_traversal})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(LinkList_src)
add_subdirectory(subs) 
add_library( LinkList ${${projectName}_policy} ${LinkList_src} )
target_link_libraries(LinkList Io)
set_property(TARGET Io PROPERTY CXX_STANDARD 11)
set_property(TARGET Io PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Io PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

file(GLOB ans_files . *.ans)
file(COPY ${ans_files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list( APPEND tests
ArrayTest
ArrayTest2
ListAccessTest
ListTest
NameArrayTest
ReferenceListTest
)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} LinkList)
  add_test(${test} ${test})
endforeach(test)

#ctest( ArrayTest COMMAND ${CMAKE_COMMAND} -E compare_files ArrayTest.out
#    WORKING_DIRECTORY ${WORK_DIR}
#    RESULT_VARIABLE CompareResult
#)

list(APPEND INSTALL_TARGETS LinkList)
set(INSTALL_TARGETS PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting LinkList subdirectory")
endif(${verbose_traversal})
