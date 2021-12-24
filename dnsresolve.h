#ifndef DNSRESOLVE_H_INCLUDED
#define DNSRESOLVE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>


char* dns_resolve(char* domain);


#endif // DNSRESOLVE_H_INCLUDED
