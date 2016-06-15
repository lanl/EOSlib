if(${verbose_traversal})
  message(STATUS "Entered blessed_output subdirectory")
endif(${verbose_traversal})

list(APPEND out_files testdb data0)
foreach(out_file ${out_files})
  configure_file(${out_file}out.blessed ${CMAKE_CURRENT_BINARY_DIR}/../${out_file}out.blessed COPYONLY)
  configure_file(${out_file}err.blessed ${CMAKE_CURRENT_BINARY_DIR}/../${out_file}err.blessed COPYONLY)
endforeach(out_file)
  
if(${verbose_traversal})
  message(STATUS "Exiting blessed_output subdirectory")
endif(${verbose_traversal})
