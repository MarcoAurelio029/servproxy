#include <stdio.h>
#include <stdlib.h>

#include "serv_proxy.h"

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Erro no comando.\nTente: ./servproxy <tempo em segundos>\n");
        return -1;
    }

    int cache_timeout = atoi(argv[1]);

    init_serv_proxy((long unsigned)cache_timeout);

    return 0;
}
