// plugin module
// g++ plugin.cpp


#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dirent.h>
#include <dlfcn.h>
#endif // _WIN32

#include <unistd.h>
#include <signal.h>
//#include <execinfo.h>
//#include "sys/signal.h"

//#include "x86_64-signal.h"


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <exception>
#include <map>
#include <memory>

#include "../base/Singleton_t.h"
#include "../base/export.h"


#ifndef _MSC_VER
#define _FUNC __PRETTY_FUNCTION__
#else // _MSC_VER
#define _FUNC __FUNCTION__
#endif // _MSC_VER


const char* Location(const char *file_name, int line_num, const char *func_name) {
  stringstream ss;
  ss << file_name << ": " << line_num << ": " << func_name << ": ";
  return ss.str().c_str();
}


#define M_LOCATION Location((__FILE__), (__LINE__), (_FUNC))


struct Main_args {
  int argc;
  char **argv;
  char **envs;

  Main_args(int _argc = 0, char **_argv = 0, char **_envs = 0) :
    argc(_argc), argv(_argv), envs(_envs)
  {}

  Main_args(const Main_args &r) : argc(r.argc), argv(r.argv), envs(r.envs)
  {}

  Main_args& operator= (const Main_args &r) {
    if (this != &r) {
      argc = r.argc;
      argv = r.argv;
      envs = r.envs;
    }
    return *this;
  }
};


namespace Dynlib {
  using namespace std;


  struct Error {
    string msg;

    Error() throw ()
    {}

    virtual ~Error() throw ()
    {}

    virtual const char* what() const throw() {
      return msg.c_str();
    }
  };


  struct Open_lib_err : public Error {
    Open_lib_err(const string &location, const string &name) {
      msg = location + string("Can`t open library `") + name + '`';
    }
  };


  struct Get_proc_err : public Error {
    Get_proc_err(const string &location, const string &name) {
      msg = location + string("Can`t get procedure`") + name + '`';
    }
  };


  struct Iface_type_err : public Error {
    Iface_type_err(const string &location, const string &name) {
      msg = location + string("Can`t find iface by curr type`") + name + '`';
    }
  };


  class Library {
    #ifdef _WIN32
    #define M_LIB_OPEN(pLibName) LoadLibrary((pLibName))
    #define M_LIB_GET_PROC(pLib, pLibName) GetProcAddress((pLib),(pLibName))
    #define M_LIB_ERROR GetLastError()
    #define M_LIB_CLOSE(pLib) FreeLibrary((pLib))
    #else // _WIN32
    #define M_LIB_OPEN(pLibName) dlopen((pLibName), RTLD_LAZY)
    #define M_LIB_GET_PROC(pLib, pLibName) dlsym((pLib),(pLibName))
    #define M_LIB_ERROR dlerror()
    #define M_LIB_CLOSE(pLib) dlclose((pLib))
    #endif // _WIN32

    #ifdef _WIN32
    typedef HINSTANCE Handler;
    #else // _WIN32
    typedef void* Handler;
    #endif // _WIN32

    string m_name;
    Handler m_handler;

  public:
    Library(const string &name) : m_name(name), m_handler(0) {
      if ( ! (m_handler = M_LIB_OPEN(name.c_str()))) {
        throw Open_lib_err(M_LOCATION, name);
      }
    }

    virtual ~Library() {
      if (m_handler) {
        M_LIB_CLOSE(m_handler);
      }
    }

    template< class Func_type_ptr >
    Func_type_ptr get_func_ptr(const string &name) throw (Error) {
      if (Func_type_ptr ptr = (Func_type_ptr)M_LIB_GET_PROC(m_handler, name.c_str())) {
        return ptr;
      }
      else {
        throw Get_proc_err(M_LOCATION, name);
      }
    }

    const string& name() const throw() {
      return m_name;
    }
  };


  static const char *const m_plugin_main_func_name = "Main";


  class Plugin {
    typedef void(*plugin_main_ptr)(Plugin*);

    typedef map< string, Plugin* >  Map_plugins;
    typedef Map_plugins::iterator   Map_plugins_iter;
    typedef pair< string, Plugin* > Map_plugins_pair;

    template< class Type >
    struct Find_iface {
      bool operator() (const Map_plugins_pair &r) const {
        return r.second->get_iface(&typeid(Type)) != 0;
      }
    };

    struct Type_info_equal {
      bool operator() (const type_info *a, const type_info *b) {
        return a->before(*b);
      }
    };

    typedef map< const type_info*, void*, Type_info_equal > Map_ifaces;
    typedef Map_ifaces::iterator   Map_Ifaces_iter;
    typedef Map_ifaces::value_type Value_type_Iface;

    Map_ifaces m_ifaces;
    Map_plugins *m_plugins_ptr;
    auto_ptr< Library > m_lib_ap;

    void add_iface(const type_info *type_p, void *iface_p) throw (exception, Error) {
      Map_Ifaces_iter mi(m_ifaces.find(type_p));

      if (mi == m_ifaces.end()) {
        m_ifaces.insert(Value_type_Iface(type_p, iface_p));
      }
      else {
        throw Iface_type_err(M_LOCATION, type_p->name());
      }
    }

    void* get_iface(const type_info *type_p) throw (exception) {
      Map_Ifaces_iter mi(m_ifaces.find(type_p));

      if (mi != m_ifaces.end()) {
        return mi->second;
      }
      return 0;
    }

    void del_iface(const type_info *type_p) throw (exception, Error) {
      Map_Ifaces_iter mi(m_ifaces.find(type_p));

      if (mi != m_ifaces.end()) {
        m_ifaces.erase(mi);
      }
      else {
        throw Iface_type_err(M_LOCATION, type_p->name());
      }
    }

  protected:
    Main_args m_args;

  public:
    Plugin(const Main_args &args = Main_args()) : m_plugins_ptr(new Map_plugins), m_args(args) {
    }

    Plugin(Plugin *parent_ptr, auto_ptr< Library > lib_ap) :
      m_plugins_ptr(parent_ptr->m_plugins_ptr), m_lib_ap(lib_ap), m_args(parent_ptr->m_args) {
    }

    template< class Type >
    void add_iface(Type *ptr) throw (exception, Error) {
      add_iface(&typeid(Type), static_cast< void* >(ptr));
    }

    template< class Type >
    Type* get_iface() throw (exception, Error) {
      Map_plugins_iter mi(
        find_if(m_plugins_ptr->begin(), m_plugins_ptr->end(), Find_iface< Type >()));

      if (mi != m_plugins_ptr->end()) {
        return reinterpret_cast< Type* >(mi->second);
      }
      else {
        throw Iface_type_err(M_LOCATION, typeid(Type).name());
      }
    }

    template< class Type >
    void del_iface() throw (exception, Error) {
      del_iface(&typeid(Type));
    }

    virtual void load(const char *name) throw (exception, Error) {
      Plugin *plg_ptr  = new Plugin(this, new Library(name));

      plugin_main_ptr main_ptr =
        lib_ptr->get_func_ptr< plugin_main_ptr >(m_plugin_main_func_name);

      if (main_ptr) {
        main_ptr(parent_ptr);
      }
    }

    virtual void close(const char *name) throw (exception, Error) {
      Map_plugins_iter mi(m_plugins.find(name));

      if (mi != m_plugins.end()) {
        delete mi->second;
        m_plugins.erase(mi);
      }
    }
  };
};


struct Test_err : public Dynlib::Error{
  Test_err(const std::string &location, const std::string &mes) {
    msg = location + std::string("Test error: `") + mes + "`\n";
  }
};


struct Test {
  Test() {
    throw Test_err(M_LOCATION, "Test library");
  }

  ~Test() {
    std::cout << M_LOCATION << "Test library complete!\n" << flush;
  }
};


extern "C" EXPORT void Main(Dynlib::Plugin *plg) {
  try {
    Test test;
    int i=1, j= 0;
    int n = i/j;
    *(int*)0 = 0;
  }
  catch (Test_err &e) {
    std::cout << e.what() << std::endl << flush;
  }
}


#ifdef __GNUC__


#include <execinfo.h>

class Sigexcept {
  static void print_trace(void) {
    void *array[10];
    size_t size;
    char **strings;
    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    printf("Obtained %zd stack frames.\n", size);

    for (size_t i = 0; i < size; i++) {
      printf ("%s\n", strings[i]);
    }
    free(strings);
  }

  static void init(int sig_num) {
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, sig_num);
    sigprocmask(SIG_UNBLOCK, &sigs, 0);
    print_trace();
  }

  static void unblock_hup_signal(int sig_num __attribute__((__unused__))) throw (std::exception) {
    init(sig_num);
    throw std::runtime_error("Hup error.");
  }

  static void unblock_term_signal(int sig_num __attribute__((__unused__))) throw (std::exception) {
    init(sig_num);
    throw std::runtime_error("Term error.");
  }

  static void unblock_int_signal(int sig_num __attribute__((__unused__))) throw (std::exception) {
    init(sig_num);
    throw std::runtime_error("Int error.");
  }

  static void unblock_segv_signal(int sig_num __attribute__((__unused__))) throw (std::exception) {
    init(sig_num);
    throw std::runtime_error("Segmentation fault.");
  }

  static void unblock_fpe_signal(int sig_num __attribute__((__unused__))) throw (std::exception) {
    init(sig_num);
    throw std::runtime_error("Floating error.");
  }

public:
  Sigexcept() {
    signal(SIGSEGV, unblock_segv_signal);
    signal(SIGFPE, unblock_fpe_signal);
    signal(SIGINT, unblock_int_signal);
    signal(SIGHUP, unblock_hup_signal);
    signal(SIGTERM, unblock_term_signal);
  }
} g_sig_except;

#endif //  __GNUC__


int main(int argc, char **argv)
try {
  Singleton<Dynlib::Plugin>::set(
    std::auto_ptr<Dynlib::Plugin>(new Dynlib::Plugin(Main_args(argc, argv))));
  Dynlib::Plugin::load("./a.so");

  return EXIT_SUCCESS;
}
catch (std::bad_alloc) {
  cerr << "`new` исчерпал память.\n";
}
catch (std::exception &e) {
  e.what();
}
catch (...) {
  std::cout << "Undefined exception!\n";
  return EXIT_FAILURE;
}
//*/
