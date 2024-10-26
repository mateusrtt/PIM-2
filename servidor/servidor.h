#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <vector>
#include <string>
#include <winsock2.h> 
#include "../produto/produto.h"

/**
 * @brief  Porta utilizada para a comunicação do servidor.
 */
#define PORTA 12345

/**
 * @brief Configura o servidor para aceitar conexões.
 */
SOCKET configurarServidor();

/**
 * @brief Aceita uma conexão de um cliente.
 */
SOCKET aceitarConexao(SOCKET servidorSocket);

/**
 * @brief Carrega o estoque de produtos a partir de um arquivo.
 */
void carregarEstoque();

/**
 * @brief Inicia o servidor e aguarda conexões de clientes.
 */
void iniciarServidor();

#endif 