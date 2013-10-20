// Object test


#include <iostream>
#include "../base/Object_t.hpp"


using namespace std;


struct Test
{
  Test()
  {
    cout << "Test::Test(): p=" << this << "\n";
  }

  Test(const Test &r)
  {
    cout << "Test::Test(Test*=" << &r << "): p=" << this << "\n";
  }

  void func()
  {
    cout << "Test::func(): p=" << this << "\n";
  }
  
  Test& operator= (const Test &r)
  {
    cout << "Test::operator=(): p=" << this << "\n";
  }

};


int main(int, char**)
{
  Object obj(78);
  int i = obj;

  cout << "i= " << i << "\n";

  obj.set(Test());
 
  Test t = obj;

  t.func();
  
  //float f = obj;
  //cout << "f= " << f << "\n";
   
  return EXIT_SUCCESS;
}
