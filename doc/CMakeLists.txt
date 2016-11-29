if(${verbose_traversal})
  message(STATUS "Entered doc subdirectory")
endif(${verbose_traversal})

file(GLOB tmp  *.tex *.bib)
list(APPEND doc_files README.txt makefile ${tmp})

foreach(doc_file ${doc_files})
  file(COPY ${doc_file} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach(doc_file)

if(${verbose_traversal})
  message(STATUS "Exiting doc subdirectory")
endif(${verbose_traversal})
