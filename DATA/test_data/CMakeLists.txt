if(${verbose_traversal})
  message(STATUS "Entered test_data subdirectory")
endif(${verbose_traversal})

list(APPEND data_files DataBaseTest DataBaseUnits EosTest IdealGasTest
  StiffenedGasTest)

foreach(data_file ${data_files})
  configure_file(${data_file}.data ${CMAKE_CURRENT_BINARY_DIR}/${data_file}.data COPYONLY)
endforeach(data_file)

if(${verbose_traversal})
  message(STATUS "Exiting test_data subdirectory")
endif(${verbose_traversal})
