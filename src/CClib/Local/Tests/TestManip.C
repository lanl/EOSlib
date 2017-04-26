#include <LocalIo.h>

int main(int, char **argv)
{
  ProgName(*argv);
  std::cout << ProgName() << std::endl;
  if (std::string(ProgName()) != std::string(*argv)){
    std::cerr << Error("Test Error") << Exit;
  }
  return 0;
}
