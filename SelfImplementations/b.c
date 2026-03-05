#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

char buffer[4096];

int main(int argc, char *argv[])
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &local.sin_addr);
  local.sin_port = htons(9876);

  bind(sock, (void*)&local, sizeof(local));
  listen(sock, 128);

  int client = accept(sock,NULL, NULL);
  write(client, "Welcome\r\n", 9);

  int bytesRead = 0, res;
  for (;;) {
    res = read(client, buffer, sizeof(buffer));
    if(res < 0) {
      perror("read");
      exit(1);
      break;
    }

    if(!res) {
      break;
    }

    bytesRead += res;
  }

  printf("%d\n", bytesRead);
  return 0;
}
