#include<iostream>
#include<winsock2.h>
#include<ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 8080 
using namespace std;

int main(){
    WSADATA wsaData;

    // Inicia o winsocket 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr<<"Erro ao inicializar o winsock"<<endl;
        return -1;
    }

    // Criação do socket do servidor
    SOCKET server_fd, new_socket; 
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET){
        cerr << "Erro ao criar o socket" << endl;
         WSACleanup();
        return -1;
    }

    // Configuração do endereço do servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
     address.sin_port = htons(PORT);

    // Vincula o socket ao endereço e porta especificados
     if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR){
     cerr << "Erro ao fazer o bind" << endl;
     closesocket(server_fd);
     WSACleanup();
     return -1;
     }

     // Coloca o servidor para escutar conexões
     if (listen(server_fd, 3) == SOCKET_ERROR){
        cerr << "Erro ao colocar em modo de escuta" << endl;
        closesocket(server_fd); 
        WSACleanup();
        return -1;
     }

     cout << "Servidor em execução. Aguardando conexões..." << endl; 

return 0;
}