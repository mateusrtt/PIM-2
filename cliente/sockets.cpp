#include "sockets.h"
#include "cliente.h"
#include <iostream>
using namespace std;

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

// Função para conectar ao servidor
void conectarServidor(SOCKET clienteSocket, struct sockaddr_in& servidorAddr) {
    servidorAddr.sin_family = AF_INET; // Define a família de endereços
    servidorAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP do servidor (localhost)
    servidorAddr.sin_port = htons(PORTA); // Porta do servidor
    // Tenta conectar ao servidor
    cout << "Tentando conectar ao servidor...\n";
    if (connect(clienteSocket, (struct sockaddr*)&servidorAddr, sizeof(servidorAddr)) == SOCKET_ERROR) {
        cout << "Erro ao conectar ao servidor. Codigo de erro: " << WSAGetLastError() << "\n";
        closesocket(clienteSocket); // Fecha o socket em caso de erro
        WSACleanup(); // Limpa a Winsock
        exit(1); // Encerra o programa
    }
    cout << "Conectado ao servidor.\n"; // Mensagem de conexão bem-sucedida
}