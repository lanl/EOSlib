if(${verbose_traversal})
  message(STATUS "Entered Examples subdirectory")
endif(${verbose_traversal})
message(STATUS "Examples\n")

file(GLOB ExampleFiles  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*")
file(COPY  ${ExampleFiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

add_executable(LowLevelHugoniot LowLevelHugoniot.C)
target_link_libraries(LowLevelHugoniot EOS EOStypes)

add_executable(HugoniotDB HugoniotDB.C)
target_link_libraries(HugoniotDB EOS)


if(${verbose_traversal})
  message(STATUS "Exiting Examples subdirectory")
endif(${verbose_traversal})

