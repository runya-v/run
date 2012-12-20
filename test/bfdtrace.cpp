// bfdtrace

// g++ ./bfdtrace.cpp -finstrument-functions -lbfd -liberty

#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <map>

#include <bfd.h>
//#include <demangle.h>
#include <cxxabi.h>
//#include <windows.h>
#include <stdint.h>


using namespace std;


#define M_VALUEOF(x) ((x)->section->vma + (x)->value)


class BfdCheck
{
  typedef map< uint32_t, string >  Map;
  typedef Map::iterator            MapIter;
  typedef Map::value_type          MapVtype;
  typedef pair< uint32_t, string > Pair;

public:
  BfdCheck(const char *pName) : m_result(false), m_abfd(bfd_openr(pName, 0))
  {
    clog << m_abfd << endl;

    if (m_abfd != 0)
    {
      if ( ! bfd_check_format(m_abfd, bfd_object))
      {
        cerr << "invalid format" << endl;
      }
      else
      {
        int capacity = bfd_get_symtab_upper_bound(m_abfd);
        clog << capacity << endl;

        if (capacity < 0)
        {
          cerr << "invalid capacity" << endl;
        }
        else
        {
          if (capacity != 0)
          {
            asymbol** symbols = new asymbol*[capacity];

            int symbol_count = bfd_canonicalize_symtab(m_abfd, symbols);

            if (symbol_count < 0)
            {
              cerr << "invalid symbol count" << endl;
              delete[] symbols;
            }
            else
            {
              for (int i = 0; i < symbol_count; ++i)
              {
                m_mpNames.insert(MapVtype(M_VALUEOF(symbols[i]), demangl(symbols[i]->name)));
              }
              m_result = true;
            }
          }
          else
          {
            m_result = true;
          }
        }
      }
    }
  }
  
public:
  void print()
  {
    for (MapIter mi(m_mpNames.begin()); mi != m_mpNames.end(); ++mi)
    {
      clog.width(8);
      clog << hex << mi->first << "\t" << mi->second << endl;
    }
  }

  bool result() const
  {
    return m_result;
  }

private:
  char* demangl(const char *mangled)
  {
    int rv;
    const char* demangled = abi::__cxa_demangle(mangled, 0, 0, &rv);

    if (rv == 0)
    {
        std::cout << demangled << '\n';
        return (char*)demangled;
    }
    else
    {
        std::cout << mangled << '\n';
        return (char*)mangled;
    }
//    char *res;
//    const char *p;
//
//    if ((m_abfd != 0) && bfd_get_symbol_leading_char(m_abfd) == name[0])
//    {
//      ++name;
//    }
//
//    p = name;
//
//    while (*p == '.')
//    {
//      ++p;
//    }
//
//    res = cplus_demangle(p, (DMGL_ANSI | DMGL_PARAMS));
//
//    if (res)
//    {
//      size_t dots = p - name;
//
//      if (dots != 0)
//      {
//        string sres(res);
//        size_t len = sres.size() + 1;
//        char *add_dots = new char[len + dots];
//
//        int i;
//
//        for (i = 0; i < dots; ++i)
//        {
//          add_dots[i] = name[i];
//        }
//
//        for (i = 0; i < len; ++i)
//        {
//          add_dots[i + dots] = res[i];
//        }
//
//        delete[] res;
//        res = add_dots;
//      }
//      return res;
//    }
//
//    return xstrdup(name);
  }

private:
  bfd *m_abfd;
  Map m_mpNames;
  bool m_result;

};
////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int, char** argv, char**)
{
	BfdCheck bfdc(argv[0]);

  if (bfdc.result())
  {
    bfdc.print();
  }
	return EXIT_SUCCESS;
}

