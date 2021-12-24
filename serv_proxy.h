#ifndef SERV_PROXY_H_INCLUDED
#define SERV_PROXY_H_INCLUDED
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
#include <signal.h>
#include <sys/wait.h>

#include "printmesages.h"
#include "showerror.h"
#include "dnsresolve.h"


void init_serv_proxy();

#endif // SERV_PROXY_H_INCLUDED
