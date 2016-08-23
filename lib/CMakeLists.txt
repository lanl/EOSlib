if(${verbose_traversal})
  message(STATUS "Entered lib subdirectory")
endif(${verbose_traversal})

# list(APPEND lib_files ExtElasticPlastic ExtHE MaterialLib )

# foreach(lib_file ${lib_files})
#   configure_file(${lib_file}.so ${CMAKE_CURRENT_BINARY_DIR}/${lib_file}.so COPYONLY)
# endforeach(lib_file)

if(${verbose_traversal})
  message(STATUS "Exiting lib subdirectory")
endif(${verbose_traversal})
