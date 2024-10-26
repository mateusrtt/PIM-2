#include "sockets.h"
#include "cliente.h"
#include <iostream>
using namespace std;

/**
 * @brief Inicializa o socket para comunicação.
 * 
 * Esta função configura o Winsock e cria um socket TCP para o cliente.
 * 
 * @return O SOCKET criado e pronto para uso.
 * 
 * @note Em caso de falha na inicialização ou criação do socket, o programa
 * encerra a execução.
 */
SOCKET inicializarSocket() {
    WSADATA wsaData;
    SOCKET clienteSocket;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Falha ao inicializar Winsock. Codigo de erro: " << WSAGetLastError() << "\n";
        exit(1);
    }

    clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        cout << "Erro ao criar o socket. Codigo de erro: " << WSAGetLastError() << "\n";
        WSACleanup();
        exit(1);
    }
    
    return clienteSocket;
}

/**
 * @brief Conecta o socket do cliente a um servidor.
 * 
 * Esta função configura as informações do servidor e tenta estabelecer
 * uma conexão com ele.
 * 
 * @param clienteSocket O socket do cliente a ser conectado.
 * @param servidorAddr Estrutura que contém as informações do servidor,
 * incluindo endereço IP e porta.
 * 
 * @note Se a conexão falhar, o programa é encerrado.
 */
void conectarServidor(SOCKET clienteSocket, struct sockaddr_in& servidorAddr) {
    servidorAddr.sin_family = AF_INET; 
    servidorAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servidorAddr.sin_port = htons(PORTA); 

    cout << "Tentando conectar ao servidor...\n";
    if (connect(clienteSocket, (struct sockaddr*)&servidorAddr, sizeof(servidorAddr)) == SOCKET_ERROR) {
        cout << "Erro ao conectar ao servidor. Codigo de erro: " << WSAGetLastError() << "\n";
        closesocket(clienteSocket); 
        WSACleanup();
        exit(1); 
    }
    cout << "Conectado ao servidor.\n"; 
}