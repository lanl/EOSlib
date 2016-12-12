if(${verbose_traversal})
  message(STATUS "Entered Local subdirectory")
endif(${verbose_traversal})

list( APPEND subs Io Math String )
foreach(sub ${subs})
  set(${sub}_src)
  add_subdirectory(${sub})
  add_library( ${sub} ${${projectName}_policy} ${${sub}_src} )
  set_property(TARGET ${sub} PROPERTY CXX_STANDARD 11)
  set_property(TARGET ${sub} PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET ${sub} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
endforeach(sub)
#add_subdirectory(Test)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TestIo Test.C)
target_link_libraries(TestIo Io)
add_test(NAME TestIo COMMAND TestIo)

add_executable(TestError TestError.C)
target_link_libraries(TestError Io)
add_test(NAME TestError COMMAND TestError)
 
add_executable(TestManip TestManip.C)
target_link_libraries(TestManip Io) 
add_test(NAME TestManip COMMAND TestManip)

add_executable(TestFunc TestFunc.C)
target_link_libraries(TestFunc Math )
add_test(NAME TestFunc COMMAND TestFunc )

add_executable(TestCat TestCat.C)
target_link_libraries(TestCat String Io) 
add_test(NAME TestCat COMMAND TestCat )

add_executable(TestSplit TestSplit.C)
target_link_libraries(TestSplit String Io) 
add_test(NAME TestSplit COMMAND TestSplit)

add_executable(teststrlen teststrlen.C)
target_link_libraries(teststrlen String) 
add_test(NAME teststrlen COMMAND teststrlen)

list(APPEND INSTALL_TARGETS ${subs})
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)
message(STATUS "CONTENTS OF INSTALL_TARGETS:  ${INSTALL_TARGETS}")

install(TARGETS ${subs}
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)


if(${verbose_traversal})
  message(STATUS "Exiting Local subdirectory")
endif(${verbose_traversal})
