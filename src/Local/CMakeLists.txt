if(${verbose_traversal})
  message(STATUS "Entered Local subdirectory")
endif(${verbose_traversal})

set(Io_src)
add_subdirectory(Io)

#set(Math_src)
#add_subdirectory(Math)

#set(String_src)
#add_subdirectory(String)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
 
add_library( Io ${${projectName}_policy} ${Io_src} )
set_property(TARGET Io PROPERTY CXX_STANDARD 11)
set_property(TARGET Io PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Io PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

#add_executable(TestIo Test.C)
#target_link_libraries(TestIo Io)
#add_test(TestIo TestIo)

#add_executable(TestError TestError.C)
#target_link_libraries(TestError Io)
#add_test(TestError TestError)
 
#add_executable(TestManip TestManip.C)
#target_link_libraries(TestManip Io) 
#add_test(TestManip TestManip)

#add_library( Math ${${projectName}_policy} ${Math_src} )
#set_property(TARGET Math PROPERTY CXX_STANDARD 11)
#set_property(TARGET Math PROPERTY CXX_STANDARD_REQUIRED ON)
#set_property(TARGET Math PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

#add_executable(TestFunc TestFunc.C)
#target_link_libraries(TestFunc Math )
#add_test(TestFunc TestFunc )

#add_library( String ${${projectName}_policy} ${String_src} )
#set_property(TARGET String PROPERTY CXX_STANDARD 11)
#set_property(TARGET String PROPERTY CXX_STANDARD_REQUIRED ON)
#set_property(TARGET String PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

#add_executable(TestCat TestCat.C)
#target_link_libraries(TestCat String Io) 
#add_test(TestCat TestCat )

#add_executable(TestSplit TestSplit.C)
#target_link_libraries(TestSplit String Io) 
#add_test(TestSplit TestSplit)

#add_executable(teststrlen teststrlen.C)
#target_link_libraries(teststrlen String) 
#add_test(teststrlen teststrlen)

if(${verbose_traversal})
  message(STATUS "Exiting Local subdirectory")
endif(${verbose_traversal})
