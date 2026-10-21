#include "ft_ping.h"

unsigned short checksum(void *buf, int len) {
  unsigned short *p = buf;
  unsigned int sum = 0;

  for (; len > 1; len -= 2)
    sum += *p++;
  if (len == 1)
    sum += *(unsigned char *)p;

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  return (unsigned short)~sum;
}

double get_time_ms(void) {
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

void build_packet(char *packet, size_t packet_size, uint16_t sequence) {

  if (packet_size < sizeof(struct icmp))
    return;

  memset(packet, 0, packet_size);

  struct icmp *icmp = (struct icmp *)packet;

  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_id = htons(getpid() & 0xFFFF);
  icmp->icmp_seq = htons(sequence);
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = checksum(packet, sizeof(struct icmp));
}

int main(int argc, char *argv[]) {

  struct hostent *host;
  struct sockaddr_in dest;

  if (argc < 2) {
    fprintf(stderr, "usage: %s <host>\n", argv[1]);
    return 1;
  }

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("socket (need root or CAP_NET_RAW)");
    return 1;
  }

  char packet[PACKET_SIZE];
  build_packet(packet, sizeof(packet), 1);

  host = gethostbyname(argv[1]);
  if (!host) {
    fprintf(stderr, "cannot resovle %s\n", argv[1]);
    return 1;
  }

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_addr = *(struct in_addr *)host->h_addr;

  double start = get_time_ms();

  printf("PING %s \n", argv[1]);
  if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest,
             sizeof(dest)) <= 0) {
    perror("sendto");
    return 1;
  }

  char buf[1024];
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);

  int n =
      recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen);
  if (n < 0) {
    perror("recvfrom");
    return 1;
  }

  double end = get_time_ms();

  double rtt = end - start;

  struct ip *ip = (struct ip *)buf;
  int ip_hdr_len = ip->ip_hl * 4;
  struct icmp *reply = (struct icmp *)(buf + ip_hdr_len);

  if (reply->icmp_type != ICMP_ECHOREPLY) {
    printf("ignoring ICMP type %d\n", reply->icmp_type);
    return 1;
  }
  if (ntohs(reply->icmp_id) != (getpid() & 0xFFFF)) {
    printf("ignoring reply for another process\n");
    return 1;
  }

  printf("Got reply: %d bytes from %s seq=%d ttl=%d time=%.3f ms\n",
         n - ip_hdr_len, inet_ntoa(from.sin_addr), ntohs(reply->icmp_seq),
         ip->ip_ttl, rtt);

  return 0;
}
