// functor test
// g++ -I../ functest.cpp

#include <iostream>
#include "base/Functor_t.hpp"


using namespace std;


struct TestCall
{
  TestCall()
  {
    cout << "TestCall::TestCall\n" << flush;
  }
  
  float func_f(int i)
  {
    cout << "TestCall::func: num=1: int=" << i << "\n" << flush;
    return 7.7;
  }

  void func_v()
  {
    cout << "TestCall::func: num=0\n" << flush;
  }

};

void func_v(double d)
{
  cout << "func: num=1: double=" << d << "\n" << flush;
}

int func_i()
{
  cout << "func: num=0\n" << flush;
  return 110;
}



int main(int, char**)
{
  cout << "Functor test... \n" << flush;
  
  TestCall tcall;
  
  Functor func(&tcall, &TestCall::func_f);
  cout << "ret=" << (float)func(11) << "\n" << flush;
  
  func.set(&tcall, &TestCall::func_v);
  func();
  
  func.set(&func_v);
  func(4.7);
  
  func.set(&func_i);
  cout << "ret=" << (int)func() << "\n" << flush;
  
  cout << "end.\n" << flush;
  
  return EXIT_SUCCESS;
}
