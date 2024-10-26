#ifndef CLIENTE_H
#define CLIENTE_H

#include <winsock2.h>
#include <vector>
#include "../produto/produto.h"

/**
 * @brief Altera as cores do texto e do fundo no console.
 */
void mudaCor(int corTexto, int corFundo = 0);

/**
 * @brief Exibe o menu principal para o cliente.
 */
void menuPrincipal(SOCKET clienteSocket);

/**
 * @brief Recebe a lista de produtos do servidor.
 */
void receberProdutos(SOCKET clienteSocket, std::vector<Produto>& produtos, std::vector<Produto>& estoqueOriginal);

// Declarações de vetores globais para armazenar produtos e estoque original.
extern std::vector<Produto> produtos; 
extern std::vector<Produto> estoqueOriginal; 

#endif 