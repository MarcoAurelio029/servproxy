#include "showerror.h"

void show_error(const char* text, int res)
{
    if(res == -1)
    {
        printf(" Erro em %s: %s\n", text, strerror(errno));
        exit(1);
    }
}
