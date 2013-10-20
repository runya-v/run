// Prime number finder
// compile & test: # g++ ./primenum.cpp -lwinmm && a.exe && a.exe
// compile & test: # g++ ./extmergsort.cpp -lpthread && ./a.out && ./a.out

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <pthread.h>
#include <sys/time.h>
#endif // _WIN32

#include <sys/stat.h>

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <vector>
#include <memory>
#include <algorithm>


using namespace std;


#define SECOND_LEN                1000 // in milliseconds
#define BASE_YEAR                 1900
#define D_MAX_GENERATED_FILE_SIZE 2000
#define D_DEFAULT_FILE_NAME       "depot.bin"
#define D_DEFAULT_HANDLERS_SIZE   3
#define D_OUT_HANDLER             0
#define D_SEQUANCE_HANDLER        1
#define D_PRIME_TEST_HANDLER      2


// Get File size; recomendet -D_FILE_OFFSET_BITS=64
class FileSize
{
  FileSize();
  FileSize(const FileSize&);
  FileSize& operator= (const FileSize&);

public:
	FileSize(const char *pFileName) : size(0)
	{
		struct stat fileStat;
		int err = stat(pFileName, &fileStat);

		if (err == 0)
		{
			size = fileStat.st_size;
		}
		else
		{
			perror("stat");
		}
	}

	operator uint64_t ()
	{
		return size;
	}

private:
	uint64_t size;

};


class Timer
{
public:
  typedef time_t TTimerTimeT;
  typedef struct tm TTimerTm;

#ifdef _WIN32
public:  
  Timer()
  {
    timeBeginPeriod(1);
  }

  ~Timer()
  {
    timeEndPeriod(1);
  }
#endif // _WIN32

public:
  static void sleep(uint32_t mlseconds)
  {

    #ifdef _WIN32
    Sleep(mseconds);
    #else // _WIN32
    usleep((mlseconds)*1000);
    #endif // _WIN32
  }

  static Timer::TTimerTm* getLocalTime()
  {
    TTimerTimeT t = time(0);
    return localtime(&t);
  }

public:
  uint32_t getTicks()
  {
    #ifdef _WIN32
    return timeGetTime();

    #else // _WIN32
    struct timeval tmv;
    struct timezone *ptz;
    
    gettimeofday(&tmv, ptz);

    return tmv.tv_usec / 1000; // to milliseconds
    #endif // _WIN32
  }

};


class Thread
{
public:

  #ifdef _WIN32
  typedef CRITICAL_SECTION TCritSect;
  typedef HANDLE TThread;
  #else // _WIN32
  typedef pthread_mutex_t TCritSect;
  typedef pthread_t TThread;
  #endif // _WIN32

  struct CritSectObj
  {
    TCritSect m_cs;

    CritSectObj()
    {
      #ifdef _WIN32
      InitializeCriticalSection(&m_cs);
      #else // _WIN32
			pthread_mutex_init(&m_cs, 0);
      #endif // _WIN32
    }

    ~CritSectObj()
    {
      #ifdef _WIN32
      DeleteCriticalSection(&m_cs);
      #else // _WIN32
			pthread_mutex_destroy(&m_cs);
      #endif // _WIN32
    }

  };

  class CritSect
  {
  public:
    CritSect()
    {
      #ifdef _WIN32
      EnterCriticalSection(&sm_cso.m_cs);
      #else // _WIN32
			pthread_mutex_lock(&sm_cso.m_cs);
      #endif // _WIN32
    }

    ~CritSect()
    {
      #ifdef _WIN32
      LeaveCriticalSection(&sm_cso.m_cs);
      #else // _WIN32
			pthread_mutex_unlock(&sm_cso.m_cs);
      #endif // _WIN32
    }
    
  private:
    static CritSectObj sm_cso;

  };

public:
  virtual ~Thread()
  {
    #ifdef _WIN32
    CloseHandle(m_thread);
    #else // _WIN32
		pthread_cancel(m_thread);
		m_thread = 0;
    #endif // _WIN32
  }

  virtual void run() = 0;

  void start()
  {
    #ifdef _WIN32
    m_thread = CreateThread(
      0, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >(Thread::threadCallback), this, 0, 0);
    #else // _WIN32
		pthread_create(
			&m_thread, 0, reinterpret_cast< void*(*)(void*) >(Thread::threadCallback), this);
    #endif // _WIN32
  }

  void kill()
  {
    #ifdef _WIN32
    TerminateThread(m_thread, 0);
    #else // _WIN32
		pthread_cancel(m_thread);
		m_thread = 0;
    #endif // _WIN32
  }

private:
	static void threadCallback(Thread *pWhoThread)
	{
    pWhoThread->run();
	}

private:
  bool m_bRun;
  TThread m_thread;
  TCritSect m_cs;
  
};


Thread::CritSectObj Thread::CritSect::sm_cso;


struct PrimeNum
{
  uint8_t b0;
  uint8_t b1;
  uint8_t b2;
  uint8_t b3;
  uint8_t b4;
  uint8_t b5;

  PrimeNum() : b0(0), b1(0), b2(0), b3(0), b4(0), b5(0)
  {}

  PrimeNum(const PrimeNum &r) : b0(r.b0), b1(r.b1), b2(r.b2), b3(r.b3), b4(r.b4), b5(r.b5)
  {}

  PrimeNum(const uint64_t &u64) :
    b0(((uint8_t*)&u64)[0]),
    b1(((uint8_t*)&u64)[1]),
    b2(((uint8_t*)&u64)[2]),
    b3(((uint8_t*)&u64)[3]),
    b4(((uint8_t*)&u64)[4]),
    b5(((uint8_t*)&u64)[5])
  {}

  PrimeNum(uint8_t _b0, uint8_t _b1, uint8_t _b2, uint8_t _b3, uint8_t _b4, uint8_t _b5) :
   b0(_b0), b1(_b1), b2(_b2), b3(_b3), b4(_b4), b5(_b5)
  {}

  PrimeNum& operator= (const PrimeNum &r)
  {
    if (this != &r)
    {
      b0 = r.b0;
      b1 = r.b1;
      b2 = r.b2;
      b3 = r.b3;
      b4 = r.b4;
      b5 = r.b5;
    }
    return *this;
  }

  PrimeNum& operator= (uint64_t u64)
  {
    b0 =((uint8_t*)&u64)[0];
    b1 =((uint8_t*)&u64)[1];
    b2 =((uint8_t*)&u64)[2];
    b3 =((uint8_t*)&u64)[3];
    b4 =((uint8_t*)&u64)[4];
    b5 =((uint8_t*)&u64)[5];
    return *this;
  }

  operator uint64_t () const
  {
    uint64_t u64 = 0;
    ((uint8_t*)&u64)[0] = b0;
    ((uint8_t*)&u64)[1] = b1;
    ((uint8_t*)&u64)[2] = b2;
    ((uint8_t*)&u64)[3] = b3;
    ((uint8_t*)&u64)[4] = b4;
    ((uint8_t*)&u64)[5] = b5;
    return u64;
  }

  bool operator< (const PrimeNum &r)
  {
    if (this != &r)
    {
      return (((uint64_t)*this) < ((uint64_t)r));
    }
    return false;
  }

  bool operator== (const PrimeNum &r)
  {
    if (this != &r)
    {
      return (((uint64_t)*this) == ((uint64_t)r));
    }
    return true;
  }

};


struct GenerateRandomFile
{
  GenerateRandomFile(const string &rFileName = D_DEFAULT_FILE_NAME)
	{
		fstream fout(rFileName.c_str(), ios_base::app | ios_base::binary | ios_base::out);

		if (fout.is_open() == false)
		{
			clog << "GenerateRandomFile(): can`t create random file: `"<< rFileName << "`\n";
		}
		else
		{
			clog << "GenerateRandomFile(): start generating random file: `"<< rFileName << "`...\n";

			srand(time(0));

			for (
        uint64_t nCurrRecords = 0;
        nCurrRecords < D_MAX_GENERATED_FILE_SIZE;
        nCurrRecords += sizeof(PrimeNum))
			{
        PrimeNum pn((uint64_t)(rand()%500));
        fout << pn.b0 << pn.b1 << pn.b2 << pn.b3 << pn.b4 << pn.b5;
			}
			clog << "GenerateRandomFile(): stop generating random file.\n" << flush;
		}
	}

private:
  GenerateRandomFile();
  GenerateRandomFile(const GenerateRandomFile&);
  GenerateRandomFile& operator= (const GenerateRandomFile&);

};


class TaskHandler : public Thread
{
public:
  class Task
  {
  public:
    Task() : m_bCmplt(true)
    {}

  public:
    virtual void execute() = 0;
    
    bool isComplete()
    {
      return m_bCmplt;
    }

  protected:
    bool m_bCmplt;

  };

public:
  TaskHandler()
  {
    Thread::CritSect cs;
    clog << "TaskHandler(): id = " << (uint32_t)this << "; is created." << endl << flush;
  }

  virtual ~TaskHandler()
  {
    Thread::CritSect cs;

    while ( ! m_queue.empty())
    {
      delete m_queue.front();
      m_queue.pop();
    }
    clog << "~TaskHandler(): id = " << (uint32_t)this << "; is destroy." << endl << flush;
  }

public:
  void add(TaskHandler::Task *task)
  {
    Thread::CritSect cs;
    m_queue.push(task);
  }

  size_t getQueueSize()
  {
    size_t size;

    // sync
    {
      Thread::CritSect cs;
      size = m_queue.size();
    }
    return size;
  }

  virtual void run()
  {
    while (1)
    {
      Task *pTask = 0;

      // sync
      {
        Thread::CritSect cs;

        if ( ! m_queue.empty())
        {
          pTask = m_queue.front();
          m_queue.pop();
        }
      }

      if (pTask != 0)
      {
        pTask->execute();

        if (pTask->isComplete())
        {
          delete pTask;
        }
      }
      Timer::sleep(0);
    }
  }

private:
  queue< Task* > m_queue;

};


class FileReader
{
  FileReader();
  FileReader(const FileReader&);
  FileReader& operator= (const FileReader&);

public:
  FileReader(const char *pFileName) : m_apFsize(new FileSize(pFileName)), m_count(0)
  {
    if ((uint64_t)*m_apFsize != 0)
    {
      m_in.open(pFileName, ios_base::binary | ios_base::in);

      if ( ! m_in.is_open())
      {
        clog << "FileReader(): can`t open in file `" << pFileName << "`\n" << flush;
      }
    }
  }

  ~FileReader()
  {
    if (m_in.is_open())
    {
      m_in.close();
    }
  }


  PrimeNum* next()
  {
    if (m_in.is_open())
    {
      m_count += sizeof(PrimeNum);

      if ((uint64_t)*m_apFsize >= m_count)
      {
        m_in.read((char*)&m_currPrNum, sizeof(PrimeNum));
        return &m_currPrNum;
      }
    }
    return 0;
  }

private:
  auto_ptr< FileSize > m_apFsize;
  ifstream m_in;
  PrimeNum m_currPrNum;
  uint64_t m_count;
  
};


class FindPrimeNumSeq
{
  typedef vector< PrimeNum > TVecPns;
  typedef vector< TVecPns >  TVvecPns;
  typedef TVecPns::iterator  TViterPns;
  typedef TVvecPns::iterator TVviterPns;

  class OutTask : public TaskHandler::Task
  {
  public:
    OutTask(const string &mess) : m_message(mess)
    {}

    static void msg(const string &mess)
    {
      if (sm_pHandler != 0)
      {
        sm_pHandler->add(new OutTask(mess));
      }
    }

    virtual void execute()
    {
      clog << m_message << flush;
    }

  public:
    static TaskHandler *sm_pHandler;

  private:
    string m_message;

  };

  struct OutSeqMsg
  {
    OutSeqMsg(stringstream *p) : pSs(p)
    {}

    void operator() (const PrimeNum &rPn)
    {
      *pSs << (uint64_t)rPn << ", ";
    }

  private:
    stringstream *pSs;

  };

  class CollectSequanceTask : public TaskHandler::Task
  {
  public:
    CollectSequanceTask(const PrimeNum &rPn) : m_pn(rPn)
    {}

    static void add(const PrimeNum &rPn)
    {
      sm_pHandler->add(new CollectSequanceTask(rPn));
    }

    virtual void execute()
    {
      if ((sm_seqBuf.size() != 0) && ((uint64_t)sm_seqBuf.back().back() <= (uint64_t)m_pn))
      {
        sm_seqBuf.back().push_back(m_pn);
      }
      else
      {
        TVecPns seq;
        seq.push_back(m_pn);

        if (sm_seqBuf.size() > 0)
        {
          stringstream ss;
          ss << "seq " << sm_seqBuf.size() << ":\t\t";

          for_each(sm_seqBuf.back().begin(), sm_seqBuf.back().end(), OutSeqMsg(&ss));

          OutTask::msg(ss.str() + "\n");
          TVviterPns vviSeq = sm_seqBuf.begin() + sm_vcNumLongSeqs.back();

          if (vviSeq->size() < sm_seqBuf.back().size())
          {
            sm_vcNumLongSeqs.clear();
            sm_vcNumLongSeqs.push_back(sm_seqBuf.size() - 1);
          }
          else if (vviSeq->size() == sm_seqBuf.back().size())
          {
            sm_vcNumLongSeqs.push_back(sm_seqBuf.size() - 1);
          }
        }
        else
        {
          sm_vcNumLongSeqs.push_back(0);
          OutTask::msg("Start collection first sequance.\n");
        }
        sm_seqBuf.push_back(seq);
      }
    }

  public:
    static TaskHandler *sm_pHandler;
  
  private:
    PrimeNum m_pn;

  };

  class PrimeTestTask : public TaskHandler::Task
  {
  public:
    PrimeTestTask(const PrimeNum &rPn) : m_pn(rPn)
    {}

    static void add(const PrimeNum &rPn)
    {
      sm_pHandler->add(new PrimeTestTask(rPn));
    }

    virtual void execute()
    {
      uint64_t num = m_pn;

      if (num == 2)
      {
        OutTask::msg("2");
      }
      else if ((num > 2) && ((num % 2) == 1))
      {
        uint64_t div = 3;

        while (((num % div) != 0.0) && (num > div))
        {
          div += 2;
        }

        if (num == div)
        {
//          stringstream ss;
//          ss << num << "\n";
//          OutTask::msg(ss.str());
          CollectSequanceTask::add(m_pn);
        }
      }
    }

  public:
    static TaskHandler *sm_pHandler;

  private:
    PrimeNum m_pn;

  };

public:
  FindPrimeNumSeq(const char *pFileName)
  {
    FileReader freader(pFileName);
    OutTask::sm_pHandler             = m_aHandlers + D_OUT_HANDLER;
    PrimeTestTask::sm_pHandler       = m_aHandlers + D_PRIME_TEST_HANDLER;
    CollectSequanceTask::sm_pHandler = m_aHandlers + D_SEQUANCE_HANDLER;

    startHandlers();

    OutTask::msg("\nFindPrimeNumSeq(): start...\n\n");
    PrimeNum *pPn;

    while ((pPn = freader.next()) != 0)
    {
      PrimeTestTask::add(*pPn);
      Timer::sleep(0);
    }
    whiteThreads();

    OutTask::msg("\nFindPrimeNumSeq(): Find complete.\n\n");

    whiteThreads();
    LongestSequances();

    OutTask::msg("\nFindPrimeNumSeq(): stop.\n\n");
  }

  ~FindPrimeNumSeq()
  {
    whiteThreads();
  }

private:
  struct OutLongSeqs
  {
    void operator() (uint32_t seq)
    {
      TVviterPns vviSeq = sm_seqBuf.begin() + seq;
      stringstream ss;
      ss << "Longest sequance " << seq + 1 << ":\t\t";

      for_each(vviSeq->begin(), vviSeq->end(), OutSeqMsg(&ss));

      OutTask::msg(ss.str() + "\n");
    }
    
  };

  void LongestSequances()
  {
    if (sm_seqBuf.size() != 0)
    {
      for_each(sm_vcNumLongSeqs.begin(), sm_vcNumLongSeqs.end(), OutLongSeqs());
    }
  }

  void whiteThreads()
  {
    size_t size;

    do
    {
      size = 0;

      for (register uint32_t i = 0; i < D_DEFAULT_HANDLERS_SIZE; ++i)
      {
        size |= m_aHandlers[i].getQueueSize();
      }
      Timer::sleep(0);
    }
    while (size != 0);
  }

  void startHandlers()
  {
    for (register uint32_t i = 0; i < D_DEFAULT_HANDLERS_SIZE; ++i)
    {
      m_aHandlers[i].start();
    }
  }

private:
  static Timer sm_timer;
  static TVvecPns sm_seqBuf;
  static vector< uint32_t > sm_vcNumLongSeqs;
  TaskHandler m_aHandlers[D_DEFAULT_HANDLERS_SIZE];

};

Timer FindPrimeNumSeq::sm_timer;

TaskHandler *FindPrimeNumSeq::OutTask::sm_pHandler = 0;
TaskHandler *FindPrimeNumSeq::PrimeTestTask::sm_pHandler = 0;
TaskHandler *FindPrimeNumSeq::CollectSequanceTask::sm_pHandler = 0;

FindPrimeNumSeq::TVvecPns FindPrimeNumSeq::sm_seqBuf;
vector< uint32_t > FindPrimeNumSeq::sm_vcNumLongSeqs;


int main(int argc, char **argv)
{
  if (argc == 1)
	{
    FileSize fsize(D_DEFAULT_FILE_NAME);

    if ((uint64_t)fsize != 0)
    {
      clog
        << "main(): readed file `" << D_DEFAULT_FILE_NAME
        << "` is finded. Size=" << (uint64_t)fsize << endl << flush;
      FindPrimeNumSeq(D_DEFAULT_FILE_NAME);
    }
    else
    {
      GenerateRandomFile(D_DEFAULT_FILE_NAME);
    }
	}
	else if (argc == 2) // generating
	{
    FindPrimeNumSeq((char*)argv[1]);
	}
  else
  {
    clog << "main(): incorrect commands line!\n" << flush;
  }

  //clog << "test\n";
  return EXIT_SUCCESS;
}
