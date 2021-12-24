#ifndef SHOWERROR_H_INCLUDED
#define SHOWERROR_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#include "showerror.h"

void show_error(const char* text, int res);

#endif // SHOWERROR_H_INCLUDED
