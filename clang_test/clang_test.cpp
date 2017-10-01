
#include <stdio.h>
//#include <iostream>

extern "C" int first()
{
  return 85;
}

//extern "C" void entry()

extern "C" int main(int argc, char** argv)
{
  auto f = first();

  printf("\nclang test %d\n", f);

  //return 0;
}

