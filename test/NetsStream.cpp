// Net Strem


#include <iostream>
#include <vector>
#include <map>
#include <string>


using namespace std:


class NetHeader {
  unsigned int hash;
  unsigned short size;
  unsigned char is_grant : 1;
  unsigned short count : 15;

  union Word {
    unsigned short word;

    struct {
      unsigned char is_grant : 1;
      unsigned short count : 15;
    }
  };

public:
  NetHeader(unsigned char *buf, unsigned short size) {
    if ((sizeof(unsigned int)*2 < size) && (size <= *((unsigned int*)buf + sizeof(unsigned int)))) {
      hash = *(unsigned int*)buf;
      size = *(unsigned short*)(buf + sizeof(unsigned int));
      Word w;
      w.word = *(unsigned short*)(buf + sizeof(unsigned int) + sizeof(unsigned short));
      is_grant = w.is_grant;
      count = w.count;
      return true;
    }
    return false;
  }

  bool is_valid() {

  }

  NetHeader& operator= (const NetHeader &r) {
    if (this != &r) {
      hash = r.hash;
      size = r.size;
      is_grant = r.is_grant;
      count = r.count;
    }
    return *this;
  }
};


class NetStream {
  vector< unsigned char > m_buf;
  vector< unsigned char > m_pack;

public:
  NetStream() {
  }

  virtual ~NetStream {
  }

  void add(unsigned char *buf, unsigned short size) {
    m_buf.insert(m_buf.end(), buf, (buf + size));
  }

  vector< unsigned char >& next() {
    NetHeader(&m_buf.front());
  }
};


int main(int, char**) {
  return EXIT_SUCCESS;
}
