#include "ft_ping.h"

int main(int argc, char *argv[]) {

  struct hostent *host;
  struct sockaddr_in dest;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <host>\n", argv[1]);
    return 1;
  }

  host = gethostbyname(argv[1]);

  if (!host) {
    fprintf(stderr, "cannot resovle %s\n", argv[1]);
    return 1;
  }

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_addr = *(struct in_addr *)host->h_addr;

  printf("Resolved %s -> %s\n", argv[1], inet_ntoa(dest.sin_addr));

  return 0;
}
