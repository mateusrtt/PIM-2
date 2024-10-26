#include "servidor.h"

/**
 * @brief Função principal do servidor.
 * 
 * Carrega o estoque de produtos a partir de um arquivo e inicializa o servidor.
 * O servidor ficará aguardando conexões de clientes e processará as solicitações.
 * 
 * @return Retorna 0 se a execução for bem-sucedida.
 */
int main() {
    carregarEstoque(); 
    iniciarServidor(); 
    return 0;
}