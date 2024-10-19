#ifndef SOCKETS_H
#define SOCKETS_H
#include <winsock2.h>
#define PORTA 12345

SOCKET inicializarSocket();
void conectarServidor(SOCKET clienteSocket, struct sockaddr_in& servidorAddr);

#endif 