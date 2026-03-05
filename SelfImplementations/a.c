#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PAYLOAD 500

char buffer[PAYLOAD];
char msg[4096];

int main(int argc, char *argv[])
{
  memset(buffer, 'z', sizeof(buffer));

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in remote;
  remote.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &remote.sin_addr);
  remote.sin_port = htons(9876);

  connect(sock, (void*)&remote, sizeof(remote));

  int n = read(sock, msg, sizeof(msg));
  if (n > 0) {
    write(1, msg, n);
  }

  write(sock, buffer, PAYLOAD);

  close(sock);
  return 0;
}
