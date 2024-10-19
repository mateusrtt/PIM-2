#include "cliente.h"
#include "sockets.h"

int main() {

    struct sockaddr_in servidorAddr; // Estrutura para armazenar informações do servidor
    SOCKET clienteSocket = inicializarSocket(); // Inicializa o socket do cliente
    conectarServidor(clienteSocket, servidorAddr); 
    receberProdutos(clienteSocket); 
    menuPrincipal(clienteSocket); 
    
    return 0; 
}