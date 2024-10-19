#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <iostream> 
#include <cstring> 
#include <winsock2.h> 
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#define PORTA 12345

using namespace std;

// Estrutura para representar um produto
struct Produto {
    string nome;
    float precoPorKg;
    float precoPorUnidade;
    float quantidadeKg;
    int quantidadeUnidade;
};

// Funções do servidor
SOCKET configurarServidor();
SOCKET aceitarConexao(SOCKET servidorSocket);
void carregarEstoque();
void iniciarServidor();

#endif 