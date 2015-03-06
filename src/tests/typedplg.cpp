// TypedPlugin


#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <memory>


using namespace std;

class Plugin;
  
namespace plg
{
  struct BaseTyped
  {
    virtual Plugin* operator() () = 0;
    
  };


  template< typename Type >
  struct Typed : BaseTyped
  {
    auto_ptr< Type > apType;
    
    Typed(Type *p = new Type()) : apType(p)
    {}
    
    virtual Plugin* operator() ()
    {
      return (Plugin*)apType.get();
    }
    
  };
  
};


class Plugin
{
public:
  Plugin* getTypedPlugin()
  {
    return (*sm_apTyped.get())();
  }

private:
  static auto_ptr< plg::BaseTyped > sm_apTyped;
  
};


struct TypedPlugin_A : Plugin
{
  TypedPlugin_A()
  {
    clog << typeid(this).name() << "\n" << flush;
  }

};


struct TypedPlugin_B : Plugin
{
  TypedPlugin_B()
  {
    clog << typeid(this).name() << "\n" << flush;
  }

};


auto_ptr< plg::BaseTyped > Plugin::sm_apTyped(new plg::Typed< TypedPlugin_A >(new TypedPlugin_A)); 


int main(int, char**, char**)
{
  return EXIT_SUCCESS; 
}
