#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestLocalIo
#include <LocalIo.h>
#include <boost/test/unit_test.hpp>
#include <iostream>

char *test_string = "Hello World!";
//std::string test_string=("Hello World!");

BOOST_AUTO_TEST_CASE(TestProgNameDefault){
  BOOST_CHECK(strcmp(ProgName(), std::string("").c_str()) == 0);
}

BOOST_AUTO_TEST_CASE(TestProgName){
  BOOST_CHECK(strcmp(ProgName(test_string), test_string) == 0);
}

BOOST_AUTO_TEST_CASE(TestProgNamePermanence){
  BOOST_CHECK(strcmp(ProgName(), test_string) == 0);
}

BOOST_AUTO_TEST_CASE(TestErrorMess){
  std::ostringstream strm;
  char* name=test_string;
  ErrorMess(strm, name);
  BOOST_CHECK(strcmp(strm.str().c_str(), (std::string("Error in ") + ProgName() +
					  ": " + test_string).c_str()) == 0);
}

BOOST_AUTO_TEST_CASE(TestErrorExit){
  std::ostringstream out;
  int test_n = 1;
  try{
    ErrorExit(out, test_n);
  }
  catch(std::runtime_error& e){
    if (not (std::string(e.what()) == "\n\t(Exiting with error status 1)\n\n"))
      throw;
  }
}
	
BOOST_AUTO_TEST_CASE(TestSkipTo){
  std::istringstream test_istream("ABC ABCDAB ABCDABDABDE");
  char* test_string = "ABCDABD";
  skipto(test_istream, test_string);
  std::string buffer; buffer.resize(4);
  test_istream.read(&(buffer[0]), 4);
  BOOST_CHECK(buffer == std::string("ABDE"));
  //  if (not (buffer == std::string("ABDE"))) { throw std::runtime_error(buffer); }
}

BOOST_AUTO_TEST_CASE(TestSkipToAndCopy){
  std::istringstream test_istream("ABC ABCDAB ABCDABDABDE");
  char* Test_string = "ABCDABD";
  std::ostringstream test_ostream;
  std::string test_out;
  skipto_and_copy(test_istream, test_string, test_ostream);
  BOOST_CHECK(test_ostream.str() == std::string("ABC ABCDAB ABCDABDABDE"));
  std::getline(test_istream, test_out);
  //  if(test_ostream.str() != std::string("ABC ABCDAB ")){
  //    throw std::runtime_error(test_ostream.str());
  //  }
}

