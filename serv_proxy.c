#include "serv_proxy.h"
#include "http_functions.h"

void init_serv_proxy(long unsigned cache_timeout)
{
    /// Criar socket do servidor proxy
    int sock_proxy = socket(AF_INET, SOCK_STREAM, 0);

    /// Estrutura que contem o endereço e a porta do servidor proxy
    struct sockaddr_in address_proxy;
    memset(&address_proxy, 0, sizeof(address_proxy));
    address_proxy.sin_family = AF_INET;
    address_proxy.sin_port = htons(8888);
    address_proxy.sin_addr.s_addr = inet_addr("127.0.0.1");

    /// Associar endereço com o socket (bind)
    int res = bind(sock_proxy, (struct sockaddr*)&address_proxy, sizeof(address_proxy));
    show_error("bind", res);

    printf("\n -----------------------------------------------\n");
    printf("    SERVIDOR PROXY [%s:%d] Ativado!\n", inet_ntoa(address_proxy.sin_addr), ntohs(address_proxy.sin_port));
    printf(" -----------------------------------------------\n");

    /// Esperar alguem se conectar
    res = listen(sock_proxy, 10);
    show_error("listen", res);

    for(;;)
    {
        /// Estrutura que guarda o endereço e a porta do cliente que se conectou
        struct sockaddr_in address_client;
        memset(&address_client, 0, sizeof(address_client));
        socklen_t addrlen = sizeof(address_client);

        /// Aceitar a requisição de conecção
        int sock_client = accept(sock_proxy, (struct sockaddr*)&address_client, &addrlen);
        show_error("accept", sock_client);

        printf("\n [+] Nova conexao: [%s:%d]\n\n", inet_ntoa(address_client.sin_addr), ntohs(address_client.sin_port));
        /// Criar novo processo para lidar com várias requisições
        pid_t pid = fork();
        if(pid == 0)
        {
            /// Receber dados de um socket conectado e gravar os bytes em req_client
            char req_client[1024];
            memset(req_client, 0, sizeof(1024));
            res = recv(sock_client, req_client, sizeof(req_client), 0);
            show_error("recv PXCL", res);

            printf("REQUISICAO ORIGINAL DO NAVEGADOR \n");
            printf("%s\n\n", req_client);

            /// Pegar endereço do Host
            char* host_addr = get_host_from_req(req_client, sizeof(req_client));

            /// Verificar se é uma requisição de favicon.ico e em seguida ignorar
            if(is_fav_icon_req(host_addr))
            {
                /// Ignorando requisicao do favicon.ico
                char resp_FAVICON[] = "HTTP/1.1 404 Not Found\r\n";
                res = send(sock_client, resp_FAVICON, sizeof(resp_FAVICON), 0);
                show_error("send PX", res);

                close(sock_client);
                exit(0);
            }

            char* ip_addr_server = dns_resolve(host_addr);
            char* host_path_temp = get_path_from_req(req_client, sizeof(req_client));
            char* host_path = path_verify(host_path_temp);

            /// Estrutura que contem o endereço e a porta do servidor alvo (site)
            struct sockaddr_in address_server;
            memset(&address_server, 0, sizeof(address_server));
            address_server.sin_family = AF_INET;
            address_server.sin_port = htons(80);
            address_server.sin_addr.s_addr = inet_addr(ip_addr_server);

            /// Fazer reqisicao ao servidor final
            /// Estabelecer conexão ao servidor proxy usando o endereço acima
            int sock_server = socket(AF_INET, SOCK_STREAM, 0);
            res = connect(sock_server, (struct sockaddr*)&address_server, sizeof(address_server));
            show_error("connect", res);

            /// Verificar se o site já esta salvo em cache
            char* path_folder_cache = (char*)calloc(strlen(host_addr) + strlen(host_path) + 1, sizeof(char));
            strcat(path_folder_cache, host_addr);
            strcat(path_folder_cache, host_path);

            if(cache_verify(path_folder_cache, cache_timeout))
            {
                FILE* file = fopen(path_folder_cache, "r");

                int c = 0;
                char buffer[1024];
                int i = 0;
                while((c = fgetc(file)) != EOF)
                {
                    buffer[i] = (char)c;
                    if(i == 1023)
                    {
                        res = send(sock_client, buffer, sizeof(buffer), 0);
                        show_error("send PX", res);
                        i = 0;
                        memset(buffer, 0, 1024);
                        continue;
                    }
                    i++;
                }

                /// Se o ultimo "pacote" a ser enviado tem menos de 1Kb, será ignorado pelo if no loop
                /// Por causa disso, vou reenviar o buffer quando sair do loop pra pegar alguma sobra de bytes
                res = send(sock_client, buffer, sizeof(buffer), 0);
                show_error("send PX", res);

                printf(" [+] Cache enviado com sucesso!\n");
                printf("--------------\n\n");

                fclose(file);
                close(sock_server);
                close(sock_client);

                /// Fechar processo criado
                exit(0);
            }

            char* req_client_conv = convert_client_req(req_client, sizeof(req_client));
            printf("REQUISICAO CONVERTIDA PROXY \n");
            puts(req_client_conv);

            printf(" [+] Criando arquivo de cache.\n\n");
            FILE* file = fopen_mkdir(path_folder_cache, "w");
            if(file == NULL) { printf(" [-] Erro na criação de arquivo.\n"); }

            res = send(sock_server, req_client_conv, strlen(req_client_conv), 0);
            show_error("send", res);

            char buffer_resp[1024];
            int nbytes = 0;

            printf(" [+] Repassando resposta do site para o navegador.\n");
            while((nbytes = recv(sock_server, buffer_resp,  sizeof(buffer_resp), 0)) > 0)
            {
                /// Injetar codigo HTML em tempo de execução de repasse de dados do servidor alvo para navegador
                char* html_inj = html_inject_str(path_folder_cache, buffer_resp, nbytes, "Nova em: ");
                if(html_inj != NULL)
                {
                    /// Enviar mensagem ao cliente conectado
                    res = send(sock_client, html_inj, strlen(html_inj), 0);
                    show_error("send PX", res);
                    show_error("Recv SI", nbytes);
                }
                else
                {
                    /// Enviar mensagem ao cliente conectado
                    res = send(sock_client, buffer_resp, nbytes, 0);
                    show_error("send PX", res);
                    show_error("Recv SI", nbytes);

                }

                for(int i=0; i<nbytes; i++)
                {
                    fputc(buffer_resp[i], file);
                }

                memset(buffer_resp, 0, sizeof(buffer_resp));
            }

            /// gravar arquivo com a data e hora do cache;  ex.: arquivo.html.cache
            FILE* file_time = fopen_mkdir(gen_cache_path_file(path_folder_cache), "w");
            fprintf(file_time, "%s", get_system_hour());
            fclose(file_time);

            fclose(file);

            html_inject_file(path_folder_cache, "Cache:\n");

            printf("     Terminado.\n");
            printf("--------------\n\n");

            close(sock_server);
            close(sock_client);

            /// Fechar processo criado
            exit(0);
        }

        wait(NULL);
    }

    close(sock_proxy);

    printf("\n [+] Socket encerrado.\n\n");

    return;
}





