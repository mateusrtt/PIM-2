#ifndef SOCKETS_H
#define SOCKETS_H
#include <winsock2.h>

SOCKET configurarServidor();
SOCKET aceitarConexao(SOCKET servidorSocket);

#endif 