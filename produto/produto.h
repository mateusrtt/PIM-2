#ifndef PRODUTO_H
#define PRODUTO_H
#include <string>

/**
 * @brief Estrutura que representa um produto.
 * 
 * A estrutura `Produto` contém informações sobre um item, incluindo
 * seu nome, preços e quantidades.
 */
struct Produto {
    std::string nome; ///< Nome do produto.
    float precoPorKg; ///< Preço por quilograma do produto.
    float precoPorUnidade; ///< Preço por unidade do produto.
    float quantidadeKg; ///< Quantidade do produto em quilogramas.
    int quantidadeUnidade; ///< Quantidade do produto em unidades.
};

#endif
