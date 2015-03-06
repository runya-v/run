// Types Array


#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include "../base/CompareTypes_t.hpp"


struct Base
{
  virtual ~Base() {}
};


struct Child_1 : public Base
{};

struct Child_2 : public Base
{};


struct Obj
{};


using namespace std;

struct Compare
{
  template< typename A, typename B > Compare(A*, B*)
  {
    cmpt::SuperSubclassStrict< A, B >();
  }

};

int main(int, char**, char**)
{
  Child_1 *p0 = new Child_1();
  Base *p1 = new Child_1();
  Base *p2 = new Child_2();
  Obj *p3 = 0;
  
  //const int i = dynamic_cast< Child* >(p0) == 0;
  struct Cmp
  {
    enum
    {
      value = (dynamic_cast< Child_1* >(p2) != 0) ?  1 : 0
    };
    
    Cmp(){}
  };
  
  clog << ((dynamic_cast< Child_1* >(p2) != 0) ?  1 : 0) << "\n" << flush; 
  
  //cmpt::SuperSubclass< float, int >();
  cmpt::SuperSubclassStrict< Base, Child_1 >();
  //Compare(p2, p0);
  //Cmp< int, int >();
  //CompileTimeError< dynamic_cast< A* >(p0) >();
  
  return EXIT_SUCCESS;
}
