#include "cliente.h"
#include "sockets.h"

/**
 * @brief Função principal do programa.
 *
 * Inicializa o socket do cliente, conecta ao servidor, 
 * recebe a lista de produtos e exibe o menu principal.
 *
 * @return Código de saída do programa. Retorna 0 em caso de sucesso.
 */
int main() {
    struct sockaddr_in servidorAddr; ///< Estrutura que contém informações do endereço do servidor.
    SOCKET clienteSocket = inicializarSocket(); ///< Inicializa o socket do cliente.
    conectarServidor(clienteSocket, servidorAddr); ///< Conecta ao servidor utilizando o socket.
    receberProdutos(clienteSocket, produtos, estoqueOriginal); ///< Recebe a lista de produtos do servidor.
    menuPrincipal(clienteSocket);  ///< Exibe o menu principal para o usuário.
    return 0; ///< Retorna 0 indicando que o programa finalizou com sucesso.
}