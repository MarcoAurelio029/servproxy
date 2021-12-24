#include "http_functions.h"

char* convert_client_req(char* const client_req, const int len)
{
    /// Pegar o path que vem depois do endereço do site
    char* path = get_path_from_req(client_req, len);
    int path_size = strlen(path);

    /// Alocar espaço suficiente para concatenar o padrao
    /// do header HTTP e o path juntos (16)
    char* header = (char*)calloc(path_size + 16, sizeof(char));
    strcat(header,"GET ");
    strcat(header,path);
    strcat(header, " HTTP/1.1");
    int header_size = strlen(header);

    char* host;
    int host_size = 0;  // Tamanho da string
    int host_index = 0; // Indice que indica onde começa a substring na string principal
    /// Definir o tamanho da string que gusrdara o endereço do host
    for(int i=0; i<len; i++)
    {
        if(client_req[i] == ' ')
        {
            host_index = i+2;
            for(int j=i+2; j<len; j++)
            {
                if(client_req[j] == ' ') { break; }
                host_size++;
            }
            break;
        }
    }

    /// Alocar string host com tamanho definido acima; o numero 6 é para incluir "Host: " no começo
    host = (char*)calloc(host_size, sizeof(char));


    for(int i=0; i<host_size; i++)
    {
        host[i] = client_req[i+host_index];
    }

    char* buffer = get_host_from_req(client_req, len);

    char* host_only = (char*)calloc(strlen(buffer) + 6, sizeof(char));
    host_only[0] = 'H'; host_only[1] = 'o'; host_only[2] = 's'; host_only[3] = 't'; host_only[4] = ':'; host_only[5] = ' ';

    strcat(host_only, buffer);

    /// Pegar o restante da string; a partir da 3ª linha (body)
    char* body;
    int body_size = 0;
    int body_index = 0;

    /// Definir o tamanho da string body que conterá o restante da requisição
    /// O len-3 serve para evitar que pegue as ultimas quebras de linha da requisição
    int cont = 0;
    for(int i=0; i<len-3; i++)
    {
        if(client_req[i] == '\n') { cont++; }
        if(cont > 1)
        {
            if(body_index == 0){ body_index = i+1; }
            body_size++;
        }
    }

    /// Alocar string que contem o restante da requisição HTTP com o tamanho encontrado acima
    body = (char*)calloc(body_size, sizeof(char));

    for(int i=0; i<body_size; i++)
    {
        body[i] = client_req[i + body_index];
    }

    /// Pocurar a tag 'acept-encoding' e mudar o valor para 'identity'
    /// Isso vai avisar para o servidor alvo que eu não quero compactação
    /// Capturar o ponteiro de onde a tag está com o strstr()
    char* p = strstr(body, "Accept-Encoding: ");
    char new_tag[] = "Accept-Encoding: identity";
    for(int i=0;i<body_size;i++)
    {
        if(&(body[i]) == p)
        {
            for(int j=0;body[j+i+1] != '\n' ; j++)
            {
                if(j < strlen(new_tag))
                {
                    body[j+i] = new_tag[j];
                } else
                {
                    body[j+i] = ' ';
                }
            }
        }
    }

    /// Juntar as 3 strings e retornar; req_total_size + 12 para incluir as quebras de linha e o \0
    int req_total_size = header_size + host_size + body_size;
    char* req_total = (char*)calloc(req_total_size + 12, sizeof(char));
    strcat(req_total, header);
    strcat(req_total, "\r\n");
    strcat(req_total, host_only);
    strcat(req_total, "\r\n");
    strcat(req_total, body);
    strcat(req_total, "\r\n\r\n\0");

    return req_total;
}


char* get_host_from_req(char* const client_req, const int len)
{
    char* host;
    int host_size = 0;  // Tamanho da string
    int host_index = 0; // Indice que indica onde começa a substring na string principal

    /// Definir o tamanho da string que guardara o endereço do host
    for(int i=0; i<len; i++)
    {
        if(client_req[i] == ' ')
        {
            host_index = i+2;
            for(int j=i+2; j<len; j++)
            {
                if(client_req[j] == ' ') { break; }
                host_size++;
            }
            break;
        }
    }

    /// Alocar string host com tamanho definido acima; o numero 6 é para incluir "Host: " no começo
    /// host_size + 3 para incluir o \0 no final (margem de segurança)
    host = (char*)calloc(host_size + 3, sizeof(char));

    for(int i=0; i<host_size; i++)
    {
        host[i] = client_req[i+host_index];
    }

    host[host_size] = '\0';

    /// Eliminar o path da URL se houver
    /// Definir o tamanho da string ate o '/' para fazer a alocação
    int host_only_size = 0;
    for(int i=0;i<host_size;i++)
    {
        if(host[i] == '/'){ break; }
        host_only_size++;
    }

    /// fazer a alocação; host_only_size + 2 para o '\0'
    char* host_only = (char*)calloc(host_only_size + 2, sizeof(char));

    /// Preencher a variavel
    for(int i=0;i<host_only_size;i++)
    {
        host_only[i] = host[i];
    }

    host_only[host_only_size + 1] = '\0';

    return host_only;
}


char* get_path_from_req(char* const client_req, const int len)
{
    char* host;
    int host_size = 0;  // Tamanho da string
    int host_index = 0; // Indice que indica onde começa a substring na string principal

    /// Definir o tamanho da string que gusrdara o endereço do host
    for(int i=0; i<len; i++)
    {
        if(client_req[i] == ' ')
        {
            host_index = i+2;
            for(int j=i+2; j<len; j++)
            {
                if(client_req[j] == ' ') { break; }
                host_size++;
            }
            break;
        }
    }

    /// Alocar string host com tamanho definido acima; o numero 6 é para incluir "Host: " no começo
    /// host_size + 3 para incluir o \0 no final (margem de segurança)
    host = (char*)calloc(host_size + 3, sizeof(char));

    for(int i=0; i<host_size; i++)
    {
        host[i] = client_req[i+host_index];
    }

    host[host_size] = '\0';

    int index = 0;
    for(int i=0;i<host_size;i++)
    {
        if(host[i] == '/')
        {
            index = i;
            break;
        }
    }

    /// Se o index for 0, significa que o endereço não tem nenhum path
    /// então será retornado apenas "/"
    if(index == 0){ return "/"; }

    char* path = (char*)calloc((host_size - index) + 2, sizeof(char));

    for(int i=0;i<(host_size - index) + 2;i++)
    {
        path[i] = host[i + index];
    }

    path[(host_size - index) + 1] = '\0';

    return path;
}


/// Função usada para verificar solicitãções de 'favicon.ico'
/// Se for esse tipo de solicitação, será enviada uma resposta
/// genérica para o navegador.
int is_fav_icon_req(char* const req)
{
    if(strcmp(req, "favicon.ico") == 0)
    {
        return 1;
    }
    return 0;
}


/// Cria apenas pastas e subpastas
void rek_mkdir(char *path) {
    char *sep = strrchr(path, '/');
    if(sep != NULL) {
        *sep = 0;
        rek_mkdir(path);
        *sep = '/';
    }
    if(mkdir(path, 0777) && errno != EEXIST)
        printf("error while trying to create '%s'\n%m\n", path);
}
/// Usado para criar arquivos nas subpastas definido no path
FILE *fopen_mkdir(char *path, char *mode) {

    /// Retirando o primeiro '/' do path
    if(path[0] == '/'){ path = &path[1]; }

    char *sep = strrchr(path, '/');
    if(sep) {
        char *path0 = strdup(path);
        path0[ sep - path ] = 0;
        rek_mkdir(path0);
        free(path0);
    }
    return fopen(path,mode);
}


char* get_system_hour()
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char* ct = (char*)calloc(64, sizeof(char));
    sprintf(ct, "%s%lu", asctime (timeinfo), rawtime);

    return ct;
}


char* gen_cache_path_file(char* path)
{
    char compl[] = ".cache";
    char* path_cache_file = (char*)calloc(strlen(path) + strlen(compl) + 1, sizeof(char));

    strcat(path_cache_file, path);
    strcat(path_cache_file, compl);

    return path_cache_file;
}


char* path_verify(char* path)
{
    if(strlen(path) != 1) { return path; }

    char* p = "/index.html";
    return p;
}


int cache_verify(char* url, long unsigned cache_timeout)
{
    /// Comparar data/hora do cache com a data atual
    char* cache_path = gen_cache_path_file(url);

    /// Tentar brir o arquivo do time do cache
    FILE* file_time = fopen(cache_path, "r");
    if(file_time == NULL){ printf( " [!] Arquivo de cache nao encontrado.\n\n"); return 0; }

    /// Extrair a data/hora do ultimo armazenamento do cache
    char date_cache_s[255];
    long unsigned date_cache_lu = 0;
    fscanf(file_time, "%[^\n]*c", date_cache_s);
    fscanf(file_time, "%lu", &date_cache_lu);

    /// Capturar data/hora atual
    time_t current_time;
    time (&current_time);

    /// comparar com a data/hora atual e ver se já expirou
    if((current_time - cache_timeout) > date_cache_lu)
    {
        printf(" [!] CACHE EXPIRADO! Obtendo versao atualizada do site.\n\n");

        return 0;
    }

    fclose(file_time);

    /// Tentar abrir os arquivos do site em cache
    FILE* file = fopen(url, "r");
    if(file != NULL)
    {
        fclose(file);
        return 1;
    }

    return 0;
}


void html_inject_file(char* path, char* msg)
{
    /// Verificar primeiro se é um arquivo .html ou .htm
    char form_1[] = ".html";
    char form_2[] = ".htm";

    int control_1 = 0;
    int control_2 = 0;
    for(int i=0;i<strlen(form_2);i++)
    {
        if(form_2[i] != path[i + (strlen(path) - 4)]){ control_1++;}
    }

    for(int i=0;i<strlen(form_1);i++)
    {
        if(form_1[i] != path[i + (strlen(path) - 5)]){ control_2++;}
    }
    if(control_1 !=0 && control_2 != 0){ return; }

    /// Verificar se o arquivo abre
    FILE* file = fopen(path, "r");
    if(file == NULL){ printf(" [-] Erro ao abrir arquivo HTML.\n"); return; }

    /// Se chegou aqui, o arquivo existe e foi aberto; prosseguir.

    char msg_pt1[] = R"(

<p style="z-index:9999; position:fixed; top:20px; left:20px; width:200px;
height:100px; background-color:yellow; padding:10px; font-weight:bold;">
)";

    /// Pegar data e hota atual
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char* currenttime = asctime (timeinfo);

    char msg_pt2[] = R"(</p>)";

    /// Concatenar tudo para formar o trecho de codigo que será injetado no HTML original
    char* str_to_inject = (char*)calloc(strlen(msg_pt1) + strlen(msg) + strlen(currenttime)  + strlen(msg_pt2) + 1, sizeof(char));
    strcat(str_to_inject, msg_pt1);
    strcat(str_to_inject, msg);
    strcat(str_to_inject, currenttime);
    strcat(str_to_inject, msg_pt2);

    /// Obter tamanho do arquivo para fazer a alocação
    int c = 0;
    int html_s = 0;
    while((c = fgetc(file)) != EOF)
    {
        html_s++;
    }
    fclose(file);

    /// Alocar a variável de acordo com o tamanho descoberto
    char* html = (char*)calloc(html_s + 1, sizeof(char));

    /// Preencher a variavel html com dados do arquivo.
    file = fopen(path, "r");
    int i = 0;
    while((c = fgetc(file)) != EOF)
    {
        html[i] = (char)c;
        i++;
    }
    fclose(file);

    /// alocar string com tamanho suficiente para o HTML original + codigo pata injetar juntos.
    char* html_inj = (char*)calloc(html_s + 2 + strlen(str_to_inject), sizeof(char));

    /// Ponteiro 'p' indica onde começa o <body>
    /// Injetar a string 'str_to_inject' dentro da string 'html_inj'.
    char* p = strstr(html, "<body");
    int offset = 0;
    for(int i=0;i<html_s;i++)
    {
        if(&(html[i]) == p)
        {
            int j = 0;
            for(j=i;html[j-1] != '>';j++)
            {
                html_inj[j] = html[j];
            }

            i = j;

            int k = 0;
            for(k=0;k<strlen(str_to_inject);k++)
            {
                html_inj[j+k] = str_to_inject[k];
            }

            offset = k;
        }
        html_inj[i+offset] = html[i];
    }

    /// Substituir o arquivo HTML original pelo conteúdo de 'html_inj'
    file = fopen(path, "w");

    for(int i=0;i<strlen(html_inj);i++)
    {
        fputc(html_inj[i], file);
    }

    fclose(file);
}


char* html_inject_str(char* path, char* html, int html_s,  char* msg)
{
    /// Verificar primeiro se a string se refere a um arquivo .html ou .htm;
    /// Isso vai definir se a string vai ser processada pela função ou não.
    char form_1[] = ".html";
    char form_2[] = ".htm";

    int control_1 = 0;
    int control_2 = 0;
    for(int i=0;i<strlen(form_2);i++)
    {
        if(form_2[i] != path[i + (strlen(path) - 4)]){ control_1++;}
    }

    for(int i=0;i<strlen(form_1);i++)
    {
        if(form_1[i] != path[i + (strlen(path) - 5)]){ control_2++;}
    }
    if(control_1 !=0 && control_2 != 0){ return NULL; }

    /// Verificar se este trecho do html contem '<body'
    char* p = strstr(html, "<body");
    if(p == NULL){ return NULL; }

    char msg_pt1[] = R"(

<p style="z-index:9999; position:fixed; top:20px; left:20px; width:200px;
height:100px; background-color:yellow; padding:10px; font-weight:bold;">
)";

    /// Pegar data e hota atual
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char* currenttime = asctime (timeinfo);

    char msg_pt2[] = R"(</p>)";

    /// Concatenar tudo para formar o trecho de codigo que será injetado no HTML original
    char* str_to_inject = (char*)calloc(strlen(msg_pt1) + strlen(msg) + strlen(currenttime)  + strlen(msg_pt2) + 1, sizeof(char));
    strcat(str_to_inject, msg_pt1);
    strcat(str_to_inject, msg);
    strcat(str_to_inject, currenttime);
    strcat(str_to_inject, msg_pt2);

    /// alocar string com codigo injetado
    char* html_inj = (char*)calloc(html_s + 2 + strlen(str_to_inject), sizeof(char));

    /// Injetar a string 'str_to_inject' dentro da string 'html_inj'.
    int offset = 0;
    for(int i=0;i<html_s;i++)
    {
        if(&(html[i]) == p)
        {
            int j = 0;
            for(j=i;html[j-1] != '>';j++)
            {
                html_inj[j] = html[j];
            }

            i = j;

            int k = 0;
            for(k=0;k<strlen(str_to_inject);k++)
            {
                html_inj[j+k] = str_to_inject[k];
            }

            offset = k;
        }
        html_inj[i+offset] = html[i];
    }

    return html_inj;
}


