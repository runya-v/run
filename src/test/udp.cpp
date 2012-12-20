// test UDP echo


#include <sys/types.h>
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>


using namespace std;


//static int g_server_port = 28000;
//static const char *const g_host_name = "localhost";
static char g_buffer[] = "Test g_buffer data";
static int g_buffer_size = sizeof(g_buffer);

/*
void server(int port) {
  cout << "start server.\n" << flush;

  SOCKET socket_id = socket(AF_INET, SOCK_DGRAM, 0);
  //SOCKET client_id = socket(AF_INET, SOCK_DGRAM, 0);
  sockaddr_in server_addr;
  sockaddr_in client_addr;
  sockaddr &server_addr_cast = (sockaddr&)server_addr;
  sockaddr &client_addr_cast = (sockaddr&)client_addr;

  // allow connection to any addr on host
  // for hosts with multiple network connections
  // and ast server port .
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  // associate process with port
  bind(socket_id, &server_addr_cast, sizeof(sockaddr_in));

  // receive from a client
  int size = sizeof(client_addr);

  //client_addr.sin_family = AF_UNSPEC;
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = INADDR_ANY;

  //while (1) {
    if (recvfrom(socket_id, g_buffer, g_buffer_size, 0, &client_addr_cast, &size) == g_buffer_size) {
      cout << "recv from: "
        << (int)client_addr.sin_addr.S_un.S_un_b.s_b1 << "."
        << (int)client_addr.sin_addr.S_un.S_un_b.s_b2 << "."
        << (int)client_addr.sin_addr.S_un.S_un_b.s_b3 << "."
        << (int)client_addr.sin_addr.S_un.S_un_b.s_b4 << ":"
        << ntohs(client_addr.sin_port) << ": `" << g_buffer << "`\n" << flush;

      HOSTENT *he = gethostbyaddr((char*)&client_addr.sin_addr, 4, AF_INET);
      g_buffer[g_buffer_size - 1] = 0;

      if (sendto(socket_id, &g_buffer[0], g_buffer_size, 0, &client_addr_cast, sizeof(sockaddr_in)) < g_buffer_size) {
        cout << "error: " << GetLastError() << ".\n";
      }
    }
  //}
  closesocket(socket_id);
  cout << "stop server.\n" << flush;
}


void client(const char *ip, int port) {
  cout << "start client.\n" << flush;

  SOCKET socket_id = socket(AF_INET, SOCK_DGRAM, 0);
  sockaddr_in server_addr;
  sockaddr_in client_addr;
  sockaddr &server_addr_cast = (sockaddr&)server_addr;
  sockaddr &client_addr_cast = (sockaddr&)client_addr;

  // specify server address , port
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  HOSTENT *hp = gethostbyname(ip);
  memcpy(&server_addr.sin_addr, hp->h_addr, hp->h_length);

  // no need to bind if not peer−to−peer
  int size = sizeof(sockaddr_in);

  if (sendto(socket_id, g_buffer, g_buffer_size, 0, &server_addr_cast, sizeof(sockaddr_in)) == g_buffer_size) {
    if (recvfrom(socket_id, g_buffer, g_buffer_size, 0, &server_addr_cast, &size) == g_buffer_size) {
      cout << "recv from: "
        << (int)server_addr.sin_addr.S_un.S_un_b.s_b1 << "."
        << (int)server_addr.sin_addr.S_un.S_un_b.s_b2 << "."
        << (int)server_addr.sin_addr.S_un.S_un_b.s_b3 << "."
        << (int)server_addr.sin_addr.S_un.S_un_b.s_b4 << ":"
        << ntohs(server_addr.sin_port) << ": `" << g_buffer << "`\n" << flush;
    }
  }
  closesocket(socket_id);
  cout << "stop client.\n" << flush;
}
//*/

typedef char U8;

struct UdpPeer {
  SOCKET sock;
  sockaddr_in addr;

  UdpPeer() {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    setBlocking();
  }

  virtual ~UdpPeer() {
    closesocket(sock);
  }

  bool setBlocking(bool is_blocking = true) {
     unsigned long notblock = ! is_blocking;

     if (ioctlsocket(sock, FIONBIO, &notblock) == -1) {
       stringstream ss;

       if (is_blocking) {
         ss << "Can`t blocking socket: error=";
       }
       else {
         ss << "Can`t not blocking socket: error=";
       }
       ss << GetLastError();
       printf(ss.str().c_str());
       return false;
     }
     return true;
  }

  sockaddr& get_addr() {
    return (sockaddr&)addr;
  }

  bool send_to(sockaddr *to_addr_ptr, vector< U8 > &buf) {
    int sended = 0;

    sockaddr_in *addr = (sockaddr_in*)to_addr_ptr;
    do {
      char *p = (char*)(&buf.front() + sended);
      int size = (buf.size() - sended);
      int cur = sendto(sock, p, size, 0, to_addr_ptr, sizeof(sockaddr_in));

      if (cur == -1) {
        stringstream ss;
        ss << "Can`t send: error=" << GetLastError();
        printf(ss.str().c_str());
        return false;
      }
      sended += cur;
    }
    while (sended < buf.size());

    return true;
  }

  bool recv_from(sockaddr *from_addr_ptr, vector< U8 > &buf) {
    int addr_size = sizeof(sockaddr_in);
    char *p = (char*)&buf.front();
    int size = buf.size();
    int recved = recvfrom(sock, p, size, 0, from_addr_ptr, &addr_size);

    sockaddr_in *addr = (sockaddr_in*)from_addr_ptr;

    if (recved == -1) {
      stringstream ss;
      ss << "Can`t recv: error=" << GetLastError();
      printf(ss.str().c_str());
      return false;
    }
    else {
      buf.resize(recved);
    }
    return true;
  }
};


class UdpServer : public UdpPeer {
public:
  UdpServer(int port) {
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int err = bind(sock, (sockaddr*)&addr, sizeof(sockaddr_in));

    if (err) {
      stringstream ss;
      ss << "Can`t bind socket with port=" << ntohs(addr.sin_port);
      printf(ss.str().c_str());
    }
  }
};


class UdpClient : public UdpPeer {
public:
  UdpClient(string &ip, int port) {
    addr.sin_port = htons(port);
    HOSTENT *hp = gethostbyname(ip.c_str());
    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
  }

  bool send_to_server(vector< U8 > &buf) {
    return send_to((sockaddr*)&addr, buf);
  }

  bool recv_from_server(vector< U8 > &buf) {
    return recv_from((sockaddr*)&addr, buf);
  }
};


void server(int port) {
  UdpServer server(port);
  sockaddr_in addr;
  vector< char > buf;

  if (server.recv_from((sockaddr*)&addr, buf)) {
    cout << "recv from: "
      << (int)addr.sin_addr.S_un.S_un_b.s_b1 << "."
      << (int)addr.sin_addr.S_un.S_un_b.s_b2 << "."
      << (int)addr.sin_addr.S_un.S_un_b.s_b3 << "."
      << (int)addr.sin_addr.S_un.S_un_b.s_b4 << ":"
      << ntohs(addr.sin_port) << ": `" << g_buffer << "`\n" << flush;
      server.send_to((sockaddr*)&addr, buf);
  }
}


void client(const char *ip, int port) {
  string ip_str(ip);
  UdpClient client(ip_str, port);
  sockaddr_in &addr = (sockaddr_in&)(UdpPeer&)client.get_addr();
  vector< char > buf;

  client.recv_from_server(buf);

  if (client.send_to_server(buf)) {
    if (client.recv_from_server(buf)) {
      cout << "recv from: "
        << (int)addr.sin_addr.S_un.S_un_b.s_b1 << "."
        << (int)addr.sin_addr.S_un.S_un_b.s_b2 << "."
        << (int)addr.sin_addr.S_un.S_un_b.s_b3 << "."
        << (int)addr.sin_addr.S_un.S_un_b.s_b4 << ":"
        << ntohs(addr.sin_port) << ": `" << g_buffer << "`\n" << flush;
    }
  }
}


int main(int argc, char **argv) {
  if (argc == 3) {
    char buf[1024] = {0};

    if (WSAStartup(0x0101, (WSADATA*)&buf[0]))
    {
      cout << "WSAStartup error: " << WSAGetLastError() << "\n" << flush;
      return EXIT_FAILURE;
    }
    else {
      if (strcmp(argv[1], "-s") == 0) {
        server(atoi(argv[2]));
        return 0;
      }

      if (strcmp(argv[1], "-c") == 0) {
        char *port_ptr = strstr(argv[2], ":");

        if (port_ptr && strlen(port_ptr) > 1) {
          string ip(argv[2], port_ptr);
          client(ip.c_str(), atoi(port_ptr + 1));
          return 0;
        }
      }
    }
  }
  cout << "Use `-s port` or `-c IP:port`.\n" << flush;
  return EXIT_SUCCESS;
}
