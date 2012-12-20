// plugin test


#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>

#include "../base/Object_t.hpp"
#include "../base/types.hpp"


using namespace std;


class Plugin;


namespace plg
{
  struct Args
  {
    Plugin *pParentPlg;
    MainArgs margs;

    Args(Plugin *parent = 0, const MainArgs ma = MainArgs()) : pParentPlg(parent), margs(ma)
    {
      clog << "plg::Args(" << (int)pParentPlg << ", ...)\n" << flush;
    }

    Args& operator= (const Args &r)
    {
      clog << "plg::Args::operator= (" << (int)pParentPlg << ", ...)\n" << flush;
      if (this != &r)
      {
        pParentPlg = r.pParentPlg;
        margs      = r.margs;
      }
      return *this;
    }

  };

  struct Specific
  {
    plg::Args args;
    auto_ptr< Plugin > apPlg;

    Specific(Plugin *plg = 0) : apPlg(plg)
    {
      clog << "plg::Specific(" << (int)apPlg.get() << ")\n" << flush;
    }

    virtual Plugin* operator() (const plg::Args &rArgs)
    {
      clog << "plg::Specific::operator() (" << (int)rArgs.pParentPlg << ", ...)\n" << flush;
      args = rArgs;
      return apPlg.get();
    }

  private:
    Specific& operator= (const Specific &r);

  };

};


class Plugin
{
  typedef map< string, Plugin* >   MapPlg;
  typedef MapPlg::iterator         MapIterPlg;
  typedef MapPlg::value_type       MapVtypePlg;
  typedef pair< MapIterPlg, bool > PairBoolPlg;

  typedef map< type_info, Object > MapObj;
  typedef MapObj::iterator         MapIterObj;
  typedef MapObj::value_type       MapVtypeObj;
  typedef pair< MapIterObj, bool > PairBoolObj;

public:
  Plugin(string name) : m_name(name)
  {
    clog << "Plugin(`" << m_name << "`)\n" << flush;
  }

  ~Plugin()
  {
    clog << "~Plugin(`" << m_name << "`)\n" << flush;
  }

public:
  static bool open(string name)
  {
    return false;
  }

  static void close(string name)
  {}

  static Plugin* get(const plg::Args &args)
  {
    clog << "Plugin::get()\n" << flush;
    Plugin *plg = (*sm_apSpecific.get())(args);
    clog << "Plugin::get(): plg = `" << (int)plg << "`; parent plg = `" << args.pParentPlg << "`\n" << flush;

    if (plg != 0)
    {
      if (args.pParentPlg != 0)
      {
        PairBoolPlg pb(args.pParentPlg->m_mpPlgs.insert(MapVtypePlg(plg->m_name, plg)));

        if ( ! pb.second)
        {
          return 0;
        }
      }
      return plg;
    }
    return 0;
  }

public:
  Plugin* clone()
  {
    Plugin *plg = new Plugin(m_name);

    if (plg)
    {
      plg->m_mpPlgs = m_mpPlgs;
      plg->m_mpPlgs = m_mpPlgs;
    }
    return plg;
  }

  string& name()
  {
    return m_name;
  }

  template< typename T >
  bool add(Object obj)
  {
    clog << "Plugin::add<" << typeid(T).name() << ">()\n" << flush;
    PairBoolObj pb(m_mpObjs.insert(make_pair(typeid(T), obj)));
    return pb.second;
  }

  template< typename T >
  T* get()
  {
    clog << "Plugin::get<" << typeid(T).name() << ">()\n" << flush;
    MapIterObj mi(m_mpObjs.find(&typeid(T)));

    if (mi != m_mpObjs.end())
    {
      return &mi->second;
    }
    return 0;
  }

  template< typename T >
  bool del()
  {
    clog << "Plugin::del<" << typeid(T).name() << ">()\n" << flush;
    MapIterObj mi(m_mpObjs.find(&typeid(T)));
    
    if (mi != m_mpObjs.end()) 
    {
      m_mpObjs.erase(mi);
      return true;
    }
    return false;
  }


private:
  static auto_ptr< plg::Specific > sm_apSpecific;

  string m_name;
  MapPlg m_mpPlgs;
  MapObj m_mpObjs;

};
////////////////////////////////////////////////////////////////////////////////////////////////////


class TestPlugin : public Plugin
{
public:
  TestPlugin() : Plugin("TestPlugin")
  {}
  
};


auto_ptr< plg::Specific > Plugin::sm_apSpecific(new plg::Specific(new Plugin("parent")));
//auto_ptr< plg::Specific > Plugin::sm_apSpecific(new plg::Specific(new TestPlugin()));


Plugin* Get(const plg::Args &plgArgs = plg::Args())
{
  clog << "Get()\n" << flush;
  return Plugin::get(plgArgs);
}
////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv, char **env)
{
  plg::Args plgArgs(0, MainArgs(argc, argv, env));
  Plugin::get(plgArgs);
  Plugin *plg = Get(plgArgs);

  clog << "main::plg = " << plg << "; " << flush;
  clog << "name=`" << plg->name() << "`\n" << flush;

  return EXIT_SUCCESS;
}
