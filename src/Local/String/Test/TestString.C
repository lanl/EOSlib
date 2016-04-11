#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestString
#include <String.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>
using namespace std;

BOOST_AUTO_TEST_CASE(TestCat){
  char* str1 = "Hello ";
  char* str2 = "World! ";
  char* str3 = "Lorem ";
  char* str4 = "Ipsum. ";
  char* str5 = "\n";
  char* str6 = "Wowzers!";
  
  BOOST_CHECK(strcmp(Cat(str1), str1) == 0);
  BOOST_CHECK(string(str1) + string(str2) == string(Cat(str1, str2)));
  BOOST_CHECK(string(str1) + string(str2) + string(str3) + string(str4)
	      + string(str5) + string(str6) == string(Cat(str1, str2, str3, str4, str5, str6)));
}

BOOST_AUTO_TEST_CASE(TestStrdup){
  char* str1 = "Lorem Ipsum.";
  string str1a(str1);
  char* str2 = Strdup(str1);
  BOOST_CHECK(str1a == string(str2));
  str1 = "Hello World!";
  BOOST_CHECK(strcmp(str1, str2) != 0);
}

BOOST_AUTO_TEST_CASE(TestSplitString){
  char * test_string = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
  char* fail_string = "Hello";
  char* success_string = "dolor";
  SplitString testSS(test_string);
  BOOST_CHECK(strcmp(test_string, testSS.WordTo(NULL)) == 0);
  SplitString testSS2(test_string);
  BOOST_CHECK(strcmp(testSS2.WordTo(fail_string), test_string) == 0);
  SplitString testSS3(test_string);
  BOOST_CHECK(strcmp(testSS3.WordTo(success_string), "Lorem ipsum ") == 0);
  char* rem = " sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
  BOOST_CHECK(strcmp(testSS3.Remainder(), rem) == 0);
}
