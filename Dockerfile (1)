FROM gcc:latest 

COPY . /usr/src/servproxy

WORKDIR /usr/src/servproxy

EXPOSE 8888

RUN gcc -Wall dnsresolve.c http_functions.c serv_proxy.c showerror.c main.c -o servproxy

