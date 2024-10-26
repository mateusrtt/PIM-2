#ifndef PRODUTO_H
#define PRODUTO_H
#include <string>

struct Produto {
    std::string nome; 
    float precoPorKg; 
    float precoPorUnidade; 
    float quantidadeKg; 
    int quantidadeUnidade;
};

#endif
