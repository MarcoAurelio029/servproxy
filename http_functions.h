#ifndef HTTP_FUNCTIONS_H_INCLUDED
#define HTTP_FUNCTIONS_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>


/// Função que converte a requisição do cliente para uma requição do Proxy
/// Ex.:
/// GET /www-net.cs.umass.edu HTTP/1.1
/// Host: localhost:8888
/// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0
/// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
///
/// vai ficar:
///
/// GET / HTTP/1.1
/// Host: www-net.cs.umass.edu
/// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0
/// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
/// Accept-Language: en-US,en;q=0.5
char* convert_client_req(char* const client_req, const int len);

/// Função recebe a string da requisição e devolve so endereço do host
/// Ex.: www.exemplo.com/t/index.html  -> www.exemplo.com
char* get_host_from_req(char* const client_req, const int len);

/// Função recebe a string da requisição e devolve o path para as pastas da requisição
/// Ex.: www.exemplo.com/t/index.html  -> /t/index.html
char* get_path_from_req(char* const client_req, const int len);

/// Verifica se a requisição é de um 'favicon.ico'
int is_fav_icon_req(char* const req);

/// Usado para criar arquivos nas subpastas definido no path
FILE *fopen_mkdir(char *path, char *mode);

/// Retorna uma string que representa a data atual e a contagem na forma numerica inteira na segunda linha
char* get_system_hour();

/// Verificar o path '/' e corrigir para '/index.html'
char* path_verify(char* path);

/// Verifica se o site ja existe em cache, e se existir, se ainda não expirou
int cache_verify(char* url, long unsigned cache_timeout);

/// Recebe uma string de path e devolve a mesma acrescida de '.cache'
/// Ex.: www.example.com/about/html >> www.example.com/about/html.cache
char* gen_cache_path_file(char* path);

/// Injetar codigo HTML no arquivo
void html_inject_file(char* path, char* msg);

/// Injetar codigo HTML em string (o 'path' so serve para verificar se a string é .html/.htm ou nao)
char* html_inject_str(char* path, char* html_str, int html_size,  char* msg);


#endif // HTTP_FUNCTIONS_H_INCLUDED
