#include <LocalMath.h>

extern "C" {
  double TestFunc()
  {
      //return Dlimits.quiet_NaN();
      //return PI;     // from LocalMath
      return NaN;     // from LocalMath
  }
}
