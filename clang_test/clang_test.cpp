
//#include <stdio.h>
//#include <iostream>

extern "C" int first()
{
  return 85;
}

//int main(int argc, char** argv)

extern "C" void entry()
{
  auto f = first();

  //printf("\nclang test %d\n", f);

  //return 0;
}

