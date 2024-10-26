#ifndef SOCKETS_H
#define SOCKETS_H
#include <winsock2.h>

/**
 * @brief Configura o servidor para aceitar conexões.
 */
SOCKET configurarServidor();

/**
 * @brief Aceita uma conexão de um cliente.
 */
SOCKET aceitarConexao(SOCKET servidorSocket);

#endif 