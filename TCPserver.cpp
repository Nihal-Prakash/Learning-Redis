#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <iostream>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

// msg Function for `fprintf`
// Why do we need this? Why cant we use fprintf striaght up?
static void msg(const char *msg) { fprintf(stderr, "%s\n", msg); }

static void die(const char *msg) {
  int err = errno;
  fprintf(stderr, "[%d]%s\n", err, msg);
  abort();
}

static void execute(int connfd) {
  char rbuf[64] = {};
  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);

  if (n < 0) {
    msg("read() error");
    return;
  }

  cout << "Client says: " << rbuf << endl;

  char *wbuf = "world";
  write(connfd, wbuf, strlen(wbuf));
}

int main(int argc, char *argv[]) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  // AF_INET is used for IPv4, use AF_INET6 for IPv6 or dual-stack sockets
  // SOCK_STREAM is for TCP. Use SOCK_DGRAM for UDP
  // The protocol specifies a paticular protocol to be used, 0 in case there is
  // only 1

  int val{1};
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  // 2nd and 3rd arguements specifies TCP behaviour
  // val is option value
  // Diffrent options use diffrent types, so the sizeof() is required

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234); // Port
  addr.sin_addr.s_addr = htonl(0);

  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv) {
    die("bind()");
  }

  rv = listen(fd, SOMAXCONN);
  if (rv) {
    die("listen()");
  }
  // SOMAXCONN = 4096 on linux

  while (true) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);

    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);

    if (connfd < 0) {
      continue; // error
    }

    execute(connfd);
    close(connfd);
  }

  return 0;
}
