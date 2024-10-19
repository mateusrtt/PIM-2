#include "sockets.h"
#include "servidor.h"

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

SOCKET aceitarConexao(SOCKET servidorSocket) {
    struct sockaddr_in clienteAddr;
    int clienteAddrLen = sizeof(clienteAddr);
    SOCKET clienteSocket = accept(servidorSocket, (struct sockaddr*)&clienteAddr, &clienteAddrLen);
    if (clienteSocket == INVALID_SOCKET) {
        cout << "Erro ao aceitar conexão. Código de erro: " << WSAGetLastError() << "\n";
    }
    return clienteSocket;
}