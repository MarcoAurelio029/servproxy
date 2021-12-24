#include "dnsresolve.h"


char* dns_resolve(char* hostname)
{
    /// Estrutura usada como "dica" para definir o tipo de busca do getaddrinfo;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     /// Qualquer familia de endereços (IPv4/IPv6);
    hints.ai_socktype = SOCK_STREAM; /// TCP;

    /// Estrutura (lista encadeada) que guardará as informacoes de endereço coletadas;
    struct addrinfo* results;
    int res = getaddrinfo(hostname, "domain", &hints, &results);
    if(res != 0) { printf("erro getaddrinfo: %s", strerror(errno)); }

    /// Ponteiro que usado para varrer a lista encadeada;
    struct addrinfo* p;
    for(p = results; p != NULL; p = p->ai_next)
    {
        /// Casting de sockaddr para sockaddr_in para capturar o end. IP;
        struct sockaddr_in* address = (struct sockaddr_in*) p->ai_addr;

        /// Para o programa retornar o primeiro end. IP válido, eu vou fazer a soma
        /// direto do valor de <sin_addr.s_addr>; se o valor for 0, é pq o end. IP é 0.0.0.0 e não válido ele o loop continua;
        /// se a soma der diferente de 0, o ip vai ser retornado;
        uint32_t addr = address->sin_addr.s_addr;
        if(addr != 0) { return inet_ntoa(address->sin_addr); }
    }

    return NULL;
}

