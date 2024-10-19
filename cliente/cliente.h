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

extern vector<Produto> produtos; // Declaração do vetor global de produtos
extern vector<Produto> estoqueOriginal; // Declaração do vetor global de estoque original

#endif 