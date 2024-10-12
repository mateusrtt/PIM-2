#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h> // Para inet_pton
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#define PORTA 12345
#define SERVER_IP "127.0.0.1" 

using namespace std;

// Estrutura para representar um produto
struct Produto {
    string nome;
    float precoPorKg;
    float precoPorUnidade;
    float quantidadeKg;
    int quantidadeUnidade;
};

void enviarProduto(SOCKET sock, const Produto& produto) {
    int nomeTamanho = produto.nome.size();
    send(sock, (char*)&nomeTamanho, sizeof(nomeTamanho), 0);
    send(sock, produto.nome.c_str(), nomeTamanho, 0);
    send(sock, (char*)&produto.precoPorKg, sizeof(produto.precoPorKg), 0);
    send(sock, (char*)&produto.quantidadeKg, sizeof(produto.quantidadeKg), 0);
    send(sock, (char*)&produto.precoPorUnidade, sizeof(produto.precoPorUnidade), 0);
    send(sock, (char*)&produto.quantidadeUnidade, sizeof(produto.quantidadeUnidade), 0);
}

void alterarProduto(SOCKET sock) {
    Produto produto;
    cout << "Digite o nome do produto a ser alterado: ";
    cin >> produto.nome;
    
    cout << "Novo preço por kg: ";
    cin >> produto.precoPorKg;
    
    cout << "Novo preço por unidade: ";
    cin >> produto.precoPorUnidade;
    
    cout << "Nova quantidade em kg: ";
    cin >> produto.quantidadeKg;
    
    cout << "Nova quantidade em unidades: ";
    cin >> produto.quantidadeUnidade;

    // Enviar sinal para o servidor de que é uma alteração
    int tipoOperacao = 1; // 1 para alterar
    send(sock, (char*)&tipoOperacao, sizeof(tipoOperacao), 0);
    
    // Enviar produto ao servidor
    enviarProduto(sock, produto);
}

void adicionarProduto(SOCKET sock) {
    Produto produto;
    cout << "Digite o nome do produto: ";
    cin >> produto.nome;
    
    cout << "Preço por kg: ";
    cin >> produto.precoPorKg;
    
    cout << "Preço por unidade: ";
    cin >> produto.precoPorUnidade;
    
    cout << "Quantidade em kg: ";
    cin >> produto.quantidadeKg;
    
    cout << "Quantidade em unidades: ";
    cin >> produto.quantidadeUnidade;

    // Enviar sinal para o servidor de que é uma adição
    int tipoOperacao = 2; // 2 para adicionar
    send(sock, (char*)&tipoOperacao, sizeof(tipoOperacao), 0);
    
    // Enviar produto ao servidor
    enviarProduto(sock, produto);
}

void removerProduto(SOCKET sock) {
    string nome;
    cout << "Digite o nome do produto a ser removido: ";
    cin >> nome;

    // Enviar sinal para o servidor de que é uma remoção
    int tipoOperacao = 3; // 3 para remover
    send(sock, (char*)&tipoOperacao, sizeof(tipoOperacao), 0);

    // Enviar nome do produto ao servidor
    int nomeTamanho = nome.size();
    send(sock, (char*)&nomeTamanho, sizeof(nomeTamanho), 0);
    send(sock, nome.c_str(), nomeTamanho, 0);
}

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;

    // Inicializa a Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Falha ao inicializar Winsock. Código de erro: " << WSAGetLastError() << "\n";
        return -1;
    }

    // Cria o socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "Erro ao criar o socket. Código de erro: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    // Configura o endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORTA);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    // Conecta ao servidor
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cout << "Erro ao conectar ao servidor. Código de erro: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    int escolha;
    while (true) {
        cout << "Escolha uma opção:\n";
        cout << "1. Adicionar Produto\n";
        cout << "2. Alterar Produto\n";
        cout << "3. Remover Produto\n";
        cout << "4. Sair\n";
        cin >> escolha;

        switch (escolha) {
            case 1:
                adicionarProduto(sock);
                break;
            case 2:
                alterarProduto(sock);
                break;
            case 3:
                removerProduto(sock);
                break;
            case 4:
                closesocket(sock);
                WSACleanup();
                return 0;
            default:
                cout << "Opção inválida.\n";
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}