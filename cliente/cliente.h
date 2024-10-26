#ifndef CLIENTE_H
#define CLIENTE_H

#include <winsock2.h>
#include <vector>
#include "../produto/produto.h"

void mudaCor(int corTexto, int corFundo = 0);
void menuPrincipal(SOCKET clienteSocket);
void receberProdutos(SOCKET clienteSocket, std::vector<Produto>& produtos, std::vector<Produto>& estoqueOriginal);

extern std::vector<Produto> produtos; 
extern std::vector<Produto> estoqueOriginal; 

#endif 