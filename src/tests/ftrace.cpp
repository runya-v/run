// t_functrace.cpp


// g++ -finstrument-functions -o ftrace ftrace.cpp -lbfd -liberty



#include <memory.h>
//#include <bfd.h>
//#include <bfdlink.h>
#include <iostream>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cxxabi.h>
#include <typeinfo>


#define D_MAX_TRACE_SHIFT 256


static uint16_t g_nShift = 0;


extern "C" void __cyg_profile_func_enter(void*, void*) __attribute__((no_instrument_function));
extern "C" void __cyg_profile_func_exit(void*, void*) __attribute__((no_instrument_function));


void __cyg_profile_func_enter(void *pThisFunc, void *pCallSite)
{
  int rv;
  const char* mangled = typeid(pThisFunc).name();
  const char* demangled = abi::__cxa_demangle(mangled, 0, 0, &rv);

  if (rv == 0)
  {
      std::cout << demangled << '\n';
  }
  else
  {
      std::cout << mangled << '\n';
  }

	for (uint16_t i = 0; i < g_nShift; ++i)
	{
		fprintf(stderr, " ");
	}
	fprintf(stderr, "%i, %p\n", g_nShift, pThisFunc);

	if (g_nShift < D_MAX_TRACE_SHIFT)
	{
		++g_nShift;
	}
}


void __cyg_profile_func_exit(void *pThisFunc, void *pCallSite)
{
	if (g_nShift > 0)
	{
		--g_nShift;
	}

	for (uint16_t i = 0; i < g_nShift; ++i)
	{
		fprintf(stderr, " ");
	}
	fprintf(stderr, "%i.\n", g_nShift);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//*/


//#include <iostream>
//#include "../base/trace.hpp"


using namespace std;


class Test
{
public:
	Test()
	{}

	~Test()
	{}

	int function1(long l)
	{
		clog << "in function 1" << "\n" << flush;
		return l;
	}


	int function2 (int ii)
	{
		clog << "in function 2" << "\n" << flush;
		return function1(ii) + 1;
	}


	int function3 (char c)
	{
		std::cout << "in function 3" << "\n" << std::flush;
		return function2(c) + 1;
	}

};
////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv, char**env)
{
  //BfdCheck((char*)argv[0]);
// 	CBfdTracer bfdt;
// 	bfdt.printSymbolTable();

	//return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
class CBfdTracer
{

//   The macro DEFAULT_TARGET is defined by Makefile. E.g.: -DTARGET='"i686-pc-linux-gnu"'.
#define DEFAULT_TARGET "i686-pc-linux-gnu"
#define DEFAULT_FILENAME "/mnt/data/develop/robo/test/a.out"

	struct SBfdData
	{
		bfd_boolean unwindInlines;  // Unwind inlined functions.
		bfd_boolean withFunctions;  // Show function names.
		bfd_boolean doDemangle;     // Demangle names.
		bfd_boolean baseNames;      // Strip directory names.
		asymbol **ppSyms;           // Symbol table.
		bfd *pBfd;
		asection *pSection;
		char *pTarget;

		SBfdData() :
			unwindInlines(true),
			withFunctions(true),
			doDemangle(true),
			baseNames(true),
			ppSyms(0),
			pBfd(0),
			pSection(0),
			pTarget(0)
		{}

	};

	struct SBfdSymInfo
	{
		bfd_vma pc;
		char *pFileName;
		char *pFuncName;
		uint32_t jLine;
		bfd_boolean found;

		SBfdSymInfo() :
			pc(0),
			pFileName(0),
			pFuncName(0),
			jLine(),
			found(false)
		{}

	};

public:
	CBfdTracer(const char *pFileName = DEFAULT_FILENAME, const char *pTarget = DEFAULT_TARGET)
	{
		m_bfdData.pTarget = new char[strlen(pTarget) + 1];
		strcpy(m_bfdData.pTarget, pTarget);

		m_BfdSymInfo.pFileName = new char[strlen(pFileName) + 1];
		strcpy(m_BfdSymInfo.pFileName, pFileName);

		bfd_init();
		setDefaultBfdTarget();

		m_bfdData.pBfd = bfd_openr(m_BfdSymInfo.pFileName, 0);//m_bfdData.pTarget);

		if ( ! m_bfdData.pBfd)
		{
			bfdNonFatal(m_BfdSymInfo.pFileName);
		}
		else
		{
			printf("bfd*: 0x%x\n", m_bfdData.pBfd);
		}
	}

	~CBfdTracer()
	{
		bfd_close(m_bfdData.pBfd);

		if (m_bfdData.pTarget)
		{
			delete[] m_bfdData.pTarget;
		}

		if (m_BfdSymInfo.pFileName)
		{
			delete[] m_BfdSymInfo.pFileName;
		}
	}


	off_t getFileSize()
	{
		struct stat statbuf;

		if (stat(m_BfdSymInfo.pFileName, &statbuf) < 0)
		{
			nonFatal(
				"Warning: could not locate '%s'. reason: %s", m_BfdSymInfo.pFileName, strerror(errno));
		}
		else if ( ! S_ISREG(statbuf.st_mode))
		{
			nonFatal("Warning: '%s' is not an ordinary file", m_BfdSymInfo.pFileName);
		}
		else
		{
			return statbuf.st_size;
		}
		return 0;
	}

	void printSymbolTable()
	{
		asymbol **ppSymTable = 0;
		long iStorageNeeded = 0;
		long iNumbSyms = 0;
		long i;

		iStorageNeeded = bfd_get_symtab_upper_bound(m_bfdData.pBfd);
		ppSymTable = (asymbol**)malloc(iStorageNeeded);
		iNumbSyms = bfd_canonicalize_symtab(m_bfdData.pBfd, ppSymTable);

		printf("iStorageNeeded = %d, iNumbSyms = %d\n", iStorageNeeded, iNumbSyms);
		for(i = 0; i < iNumbSyms; ++i)
		{
			printf("%s\n", ppSymTable[i]->name);
		}
	}


private:
	void bfdNonFatal(const char *pStr)
	{
		const char *pErrMsg = bfd_errmsg(bfd_get_error());

		if (pStr)
		{
			fprintf(stderr, "%s: %s: %s\n", m_BfdSymInfo.pFileName, pStr, pErrMsg);
		}
		else
		{
			fprintf(stderr, "%s: %s\n", m_BfdSymInfo.pFileName, pErrMsg);
		}
	}

	void setDefaultBfdTarget()
	{
		if ( ! bfd_set_default_target(m_bfdData.pTarget))
		{
			nonFatal(
				"can't set BFD default target to `%s': %s", m_bfdData.pTarget, bfd_errmsg(bfd_get_error()));
			return;
		}
		else printf("%s\n", m_bfdData.pTarget);
		return;
	}

	void report(const char *pFormat, va_list args)
	{
		fprintf(stderr, "%s: ", m_BfdSymInfo.pFileName);
		vfprintf(stderr, pFormat, args);
		putc('\n', stderr);
	}

	void nonFatal(const char *pFormat, ...)
	{
		va_list args;

		va_start(args, pFormat);
		report(pFormat, args);
		va_end(args);
	}

private:
	SBfdData m_bfdData;
	SBfdSymInfo m_BfdSymInfo;

};
//*/
