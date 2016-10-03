if(${verbose_traversal})
  message(STATUS "Entered DATA subdirectory")
endif(${verbose_traversal})

add_subdirectory(test_data)

list(APPEND data_files Davis ElasticPlastic1D EOS EOS_VT HEburn HMX HOM JWL Porous Units)

foreach(data_file ${data_files})
  configure_file(${data_file}.data ${CMAKE_CURRENT_BINARY_DIR}/${data_file}.data COPYONLY)
endforeach(data_file)

if(${verbose_traversal})
  message(STATUS "Exiting DATA subdirectory")
endif(${verbose_traversal})
