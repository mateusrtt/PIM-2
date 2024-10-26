#ifndef SOCKETS_H
#define SOCKETS_H
#include <winsock2.h>

/**
 * @brief Porta de comunicação definida para o servidor.
 */
#define PORTA 12345

/**
 * @brief Inicializa e cria um socket.
 */
SOCKET inicializarSocket();

/**
 * @brief Estabelece uma conexão com o servidor.
 * 
 * @param clienteSocket O socket do cliente que será usado para a conexão.
 * @param servidorAddr Estrutura contendo as informações do servidor.
 */
void conectarServidor(SOCKET clienteSocket, struct sockaddr_in& servidorAddr);

#endif 