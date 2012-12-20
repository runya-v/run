// author: Velichko R.N. rostislav.vel@gmail.com
// extern merging sorting

// g++ ./extmergsort.cpp -D_FILE_OFFSET_BITS=64 -lpthread && ./a.out [<name> <name> | <name>]


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <memory>

#include <stdint.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <math.h>


#define D_MAX_GENERATED_FILE_SIZE 2000000000 / sizeof(uint)

#define D_MAX_MEMORY     256000000
#define D_SORT_CEIL_SIZE (sizeof(uint) * 2)

#define M_SLEEP(mseconds) usleep((mseconds)*1000);


using namespace std;


// Base Thread
class Thread
{
public:
	struct Mutex
	{
		static pthread_mutex_t mutex;

		Mutex()
		{
			pthread_mutex_lock(&mutex);
		}

		virtual ~Mutex()
		{
			pthread_mutex_unlock(&mutex);
		}

	private:
		static struct MutexInit
		{
			MutexInit()
			{
				pthread_mutex_init(&mutex, 0);
			}

			~MutexInit()
			{
				pthread_mutex_destroy(&mutex);
			}

		} init;

	};

public:
	Thread() : m_isRun(false)
	{}

	virtual ~Thread()
	{
		M_SLEEP(10);
		pthread_cancel(m_thread);
		m_thread = 0;
		M_SLEEP(10);
	}

	void start()
	{
		m_isRun = true;
		pthread_create(
			&m_thread, 0, reinterpret_cast< void*(*)(void*) >(Thread::threadCallback), this);
		M_SLEEP(10);
	}

	bool isRun()
	{
		Mutex();
		bool run = m_isRun;
		return run;
	}

protected:
	virtual void run() = 0;

private:
	static void threadCallback(Thread *pWhoThread)
	{
		pWhoThread->run();
		Mutex();
		pWhoThread->m_isRun = false;
		M_SLEEP(100);
	}

private:
	pthread_t m_thread;
	bool m_isRun;

};


pthread_mutex_t Thread::Mutex::mutex;
Thread::Mutex::MutexInit Thread::Mutex::init;


// Get File size
struct FileSize
{
	// recomendet -D_FILE_OFFSET_BITS=64
private:
	uint64_t size;

	operator int32_t ();
	operator int16_t ();
	operator int8_t ();
	operator uint32_t ();
	operator uint16_t ();
	operator uint8_t ();

public:
	FileSize(const string &rName) : size(0)
	{
		struct stat fileStat;
		int err = stat(rName.c_str(), &fileStat);

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

};


struct GenerateRandomFile
{
	GenerateRandomFile(const string &rName)
	{
		fstream fout(rName.c_str(), ios_base::app | ios_base::binary | ios_base::out);

		if (fout.is_open() == false)
		{
			clog << "can`t create random file: `"<< rName << "`\n";
		}
		else
		{
			clog << "start generating random file: `"<< rName << "`...\n";

			srand(time(0));

			for (uint64_t nCurrRecords = 0; nCurrRecords < D_MAX_GENERATED_FILE_SIZE; ++nCurrRecords)
			{
				fout << uint(rand());
			}
			clog << "stop generating random file.\n";
		}
	}

};


class Sorting
{
	struct WriteNumToFile
	{
		WriteNumToFile(fstream &rOut, uint32_t num)
		{
			uint8_t *p = (uint8_t*)&num;

			rOut << p[0] << p[1] << p[2] << p[3];
		}

	};

	struct SortThread : public Thread
	{
		struct IsCompleted
		{
			bool operator() (Thread *p) const
			{
				return ! p->isRun();
			}

		};

		struct Remove
		{
			void operator() (Thread *p)
			{
				delete p;
			}

		};

		struct FromVecToMap
		{
			map< uint, uint > &rSort;

			FromVecToMap(map< uint, uint > &r) : rSort(r)
			{}

			void operator() (uint num)
			{
				pair< map< uint, uint >::iterator, bool > pair(
					rSort.insert(map< uint, uint >::value_type(num, 1)));

				if (pair.second == false)
				{
					map< uint, uint >::iterator mi(pair.first);
					++mi->second;
				}
			}

		};

		struct SaveSortedData
		{
			fstream &rOut;

			SaveSortedData(fstream &r) : rOut(r)
			{}

			void operator() (const pair< uint, uint > &r)
			{
				WriteNumToFile(rOut, r.first);
				WriteNumToFile(rOut, r.second);
			}
		};

		vector< uint > vcIn;
		map< uint, uint > mpSort;

	private:
		virtual void run()
		{
			string fileName;
			stringstream ss;
			ss << "_" << uint64_t(this);

			{ // file name
				Mutex();
				fileName = sm_inFileName;
			}
			remove(fileName.begin(), fileName.end(), '.');

			fileName += ss.str();

			{ // loging
				Mutex();
				clog << "start sorting by file: `" << fileName << "`\n";
			}

			// by sort
			for_each(vcIn.begin(), vcIn.end(), FromVecToMap(mpSort));

			// save
			fstream out(fileName.c_str(), ios_base::binary | ios_base::out);

			if (out.is_open() == false)
			{
				clog << "can`t open tmp file `" << fileName << "`\n";
			}
			else
			{
				for_each(mpSort.begin(), mpSort.end(), SaveSortedData(out));
				out.close();

				Mutex();
				clog << "create tmp file: `" << fileName << "`\n";
			}
		}

	};

public:
	Sorting(const string &rInName, const string &rOutName) :
		m_nInFileSize(FileSize(rInName)), m_nTreads(0)
	{
		sm_inFileName = rInName;

		clog << "input file: `" << rInName << "` size=" << m_nInFileSize << "\n";
		clog << "otput file: `" << rOutName << "`\n";

		if (m_nInFileSize != 0)
		{
			m_nMaxBuffLen = D_MAX_MEMORY;

			clog << "max thread buffer: " << m_nMaxBuffLen << "\n";

			m_in.open(rInName.c_str(), ios_base::binary | ios_base::in);

			if (m_in.is_open() == false)
			{
				clog << "can`t open in file `" << rInName << "`\n";
			}
			else
			{
				read();
				m_in.close();
			}
		}
	}

public:
	inline bool read()
	{
		uint64_t pos;
		uint64_t m_nTreads = 20;

		do
		{
			if (m_threads.size() < m_nTreads) {
				pos = m_in.tellg();

				uint64_t sub = m_nInFileSize - pos;
				uint readLen = D_MAX_MEMORY / (m_nTreads);

				readLen = (readLen / sizeof(uint)) * sizeof(uint);

				if (readLen > sub)
				{
					readLen = sub;
				}

				SortThread *pSortTh = new SortThread;
				pSortTh->vcIn.resize(readLen);
				char *p = (char*)&(pSortTh->vcIn.front());

				m_threads.push_back(pSortTh);

				m_in.read(p, readLen);

				pSortTh->start();
				M_SLEEP(10);
			}
			else
			{
				vector< Thread* >::iterator vi(
					find_if(m_threads.begin(), m_threads.end(), SortThread::IsCompleted()));

				if (vi != m_threads.end())
				{
					delete (*vi);
					m_threads.erase(vi);
				}
				M_SLEEP(100);
			}
		}
		while (pos < m_nInFileSize);

		for_each(m_threads.begin(), m_threads.end(), SortThread::Remove());
	}

private:
	static string sm_inFileName;

	fstream m_in;
	uint64_t m_nInFileSize;
	uint64_t m_currFilePos;
	uint m_nTreads;
	uint m_nMaxBuffLen;
	vector< Thread* > m_threads;

};


string Sorting::sm_inFileName;
////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv, char** env)
{
	if (argc == 3) // sortingh
	{
		Sorting(argv[1], argv[2]);
	}
	else if (argc == 2) // generating
	{
		GenerateRandomFile(string(argv[1]));
	}
	else
	{
		clog << "Use: ems <in file name> <out file nale>" << "\n";
	}//*/

	return EXIT_SUCCESS;
}


/*	fstream tmp;
	tmp.open("temp.txt", ios_base::binary | ios_base::out);
	fstream tmpin;
	tmpin.open("temp.txt", ios_base::binary | ios_base::in);

	if (tmp.is_open() == false)
	{
			clog << "Can`t open.\n";
	}
	else
	{
		for (uint n = 0; n < 100; ++n)
		{
			//tmp << uint(rand()) << n << flush;
			tmp << n << n;
		}
		tmp.close();

		for (uint n = 0; n < 100; ++n)
		{
			char a[8] = {0};
			tmpin.read(a, 8);
			clog << ((uint*)a)[0] <<  " | " << ((uint*)a)[1] << "\n";
		}
	}*/
