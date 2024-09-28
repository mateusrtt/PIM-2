#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <limits>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080
using namespace std;

int conectarServidor(int sock){
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    return connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
}

int iniciarEConectarSocket() {
    // Inicia o Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        cerr << "Erro ao inicializar o Winsock" << endl;
        return -1;
    }

    // Criação do socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Erro ao criar socket" << endl;
        WSACleanup(); 
        return -1;
    }

    // Tentativa de conexão com o servidor
    if (conectarServidor(sock) < 0){
        cerr << "Erro ao conectar ao servidor" << endl;
        cerr << "Código de erro: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }
}

void lerProduto();

void menuPrincipal() {
    system("cls");
    int opcao;

    cout << "========================\n";
    cout << "    CAIXA HORTIFRUTI    \n";
    cout << "========================\n";
    cout << "1 - Ler produto\n";
    cout << "2 - Cancelar compra\n";
    cout << "========================\n";
    cout << "Escolha uma opcao: ";

     while (!(cin >> opcao) || (opcao != 1 && opcao != 2)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Opcao invalida. Tente novamente: ";
    }

    if (opcao == 1) {
        lerProduto();
    } else if (opcao == 2) {
        cout << "Compra cancelada..." << endl;
    }
}

int main(){
    int sock = iniciarEConectarSocket();
     if (sock < 0){
        return 1;
     }

    
closesocket(sock);
WSACleanup();
return 0;

}