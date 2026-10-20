#ifndef FT_PING
#define FT_PING

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>

#define PACKET_SIZE 64
#define HEADER_SIZE (PACKET_SIZE - sizeof(struct icmphdr))

#endif
