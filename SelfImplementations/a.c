#include <stdio.h>
#include <string.h>
#include <sys/socket.h> //socket function
#include <arpa/inet.h>  //ntohs etc.
#include <unistd.h> //write

#define PAYLOAD 1000

char buffer[PAYLOAD];

int main(int argc, char *argv[])
{
  memset(buffer, 'z', sizeof(buffer));

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  
  struct sockaddr_in remote;
  remote.sin_family = AF_INET;
  inet_pton(AF_INET,"127.0.0.1", &remote.sin_addr);
  remote.sin_port = ntohs(9876);

  connect(sock, (void*)&remote, sizeof(remote));
  write(sock, buffer, PAYLOAD);

  close(sock);
  return 0;
}
