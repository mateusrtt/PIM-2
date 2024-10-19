#ifndef CLIENTE_H
#define CLIENTE_H

#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <vector>

using namespace std;

struct Produto {
    string nome; 
    float precoPorKg; 
    float precoPorUnidade; 
    float quantidadeKg; 
    int quantidadeUnidade;
};

void mudaCor(int corTexto, int corFundo = 0);
void menuPrincipal(SOCKET clienteSocket);
void receberProdutos(SOCKET clienteSocket, vector<Produto>& produtos, vector<Produto>& estoqueOriginal);

extern vector<Produto> produtos; 
extern vector<Produto> estoqueOriginal; 

#endif 