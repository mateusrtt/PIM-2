#include "sockets.h"
#include "servidor.h"
#include <iostream> 
using namespace std;

/**
 * @brief Configura o servidor para aceitar conexões.
 * 
 * Esta função inicializa a biblioteca Winsock, cria um socket
 * do tipo TCP e configura o endereço do servidor para ouvir
 * conexões na porta definida. Se ocorrer algum erro durante
 * a inicialização, criação ou configuração do socket, a função
 * retornará INVALID_SOCKET.
 * 
 * @return SOCKET do servidor configurado ou INVALID_SOCKET em caso de erro.
 */
SOCKET configurarServidor() {
    WSADATA wsaData;
    SOCKET servidorSocket;

    struct sockaddr_in servidorAddr = {0};
    servidorAddr.sin_family = AF_INET;
    servidorAddr.sin_addr.s_addr = INADDR_ANY;
    servidorAddr.sin_port = htons(PORTA);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Falha ao inicializar Winsock. Código de erro: " << WSAGetLastError() << "\n";
        return INVALID_SOCKET;
    }

    servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidorSocket == INVALID_SOCKET) {
        cout << "Erro ao criar o socket. Código de erro: " << WSAGetLastError() << "\n";
        return INVALID_SOCKET;
    }

    if (bind(servidorSocket, (struct sockaddr*)&servidorAddr, sizeof(servidorAddr)) == SOCKET_ERROR) {
        cout << "Erro ao fazer bind. Código de erro: " << WSAGetLastError() << "\n";
        closesocket(servidorSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    if (listen(servidorSocket, 1) == SOCKET_ERROR) {
        cout << "Erro ao ouvir conexões. Código de erro: " << WSAGetLastError() << "\n";
        closesocket(servidorSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    return servidorSocket;
}

/**
 * @brief Aceita uma conexão de um cliente.
 * 
 * Esta função aguarda e aceita uma conexão de um cliente
 * usando o socket do servidor. Se a aceitação da conexão falhar,
 * a função exibirá um erro, mas continuará a execução do programa.
 * 
 * @param servidorSocket O socket do servidor que está ouvindo as conexões.
 * @return SOCKET do cliente conectado ou INVALID_SOCKET em caso de erro.
 */
SOCKET aceitarConexao(SOCKET servidorSocket) {
    struct sockaddr_in clienteAddr;
    int clienteAddrLen = sizeof(clienteAddr);
    SOCKET clienteSocket = accept(servidorSocket, (struct sockaddr*)&clienteAddr, &clienteAddrLen);
    if (clienteSocket == INVALID_SOCKET) {
        cout << "Erro ao aceitar conexão. Código de erro: " << WSAGetLastError() << "\n";
    }
    return clienteSocket;
}