#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestLocalMath
#include <LocalMath.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE(TestPi){
  BOOST_CHECK(PI == 3.1415926535897932);
}

BOOST_AUTO_TEST_CASE(TestSqr){
  BOOST_CHECK(sqr(2) == 4);
}

BOOST_AUTO_TEST_CASE(TestATan2Pi){
  double real = 0.5;
  double imag = 0.5;
  BOOST_CHECK(atan2pi(imag, real) == atan2(imag, real)/PI);
}

BOOST_AUTO_TEST_CASE(TestSinCosPi){
  double x = .25;
  double s;
  double c;

  sincospi(x, &s, &c);
  BOOST_CHECK(s == sin(x*PI));
}

BOOST_AUTO_TEST_CASE(TestMinMax){
  double hi = 1.;
  double lo = .1;
  int hi_i = 1;
  int lo_i = 0;

  BOOST_CHECK(min(hi, lo) == lo);
  BOOST_CHECK(max(hi, lo) == hi);
  BOOST_CHECK(min(hi_i, lo_i) == lo_i);
  BOOST_CHECK(max(hi_i, lo_i) == hi_i);  
}

BOOST_AUTO_TEST_CASE(TestSignFunction){
  double pos = 1;
  double neg = -1;
  double zer = 0;

  BOOST_CHECK(sign(pos) == 1);
  BOOST_CHECK(sign(neg) == -1);
  BOOST_CHECK(sign(zer) == 0);
  BOOST_CHECK(sign(1) == 1);
  BOOST_CHECK(sign(-1) == -1);
  BOOST_CHECK(sign(0) == 0);
}

