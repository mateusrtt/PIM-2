#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <vector>
#include <string>
#include <winsock2.h> 
#include "../produto/produto.h"
#define PORTA 12345

SOCKET configurarServidor();
SOCKET aceitarConexao(SOCKET servidorSocket);
void carregarEstoque();
void iniciarServidor();

#endif 