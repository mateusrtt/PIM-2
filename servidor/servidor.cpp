#include "servidor.h"
#include "sockets.h"
#include <iostream> 
#include <cstring> 
#include <fstream>
#include <sstream>
using namespace std;

// Vetores globais para armazenar produtos e estoque original.
vector<Produto> produtos;

/**
 * @brief Carrega o estoque de produtos a partir de um arquivo.
 * 
 * O arquivo "estoque.txt" é lido e os produtos são armazenados
 * no vetor `produtos`. Se o arquivo não for encontrado, uma mensagem
 * de erro é exibida.
 */
void carregarEstoque() {
    ifstream arquivo("estoque.txt");  
    if (!arquivo.is_open()) {
        cout << "Arquivo de estoque não encontrado\n";
        return;
    }

    produtos.clear();  
    Produto temp;      
    char linhaBuffer[300];  

    while (arquivo.getline(linhaBuffer, sizeof(linhaBuffer))) {
        linhaBuffer[strcspn(linhaBuffer, "\n")] = 0;

        char nome[50];
        if (sscanf(linhaBuffer, "%49[^-] - R$ %f por kg - R$ %f por unidade - Quantidade: %f kg - Quantidade Unidades: %d",
            nome, &temp.precoPorKg, &temp.precoPorUnidade, &temp.quantidadeKg, &temp.quantidadeUnidade) == 5) {
            temp.nome = nome; 
            produtos.push_back(temp); 
        } else {
            cout << "Falha ao analisar a linha: " << linhaBuffer << endl;
        }
    }
    arquivo.close();  
}

/**
 * @brief Envia a lista de produtos para o cliente.
 * 
 * A quantidade de produtos e suas informações (nome, preço por kg,
 * quantidade em kg, preço por unidade e quantidade em unidades) são
 * enviadas através do socket.
 * 
 * @param clienteSocket O socket do cliente para o qual os produtos são enviados.
 */
void enviarProdutos(SOCKET clienteSocket) {
    int numeroDeProdutos = produtos.size();
    send(clienteSocket, (char*)&numeroDeProdutos, sizeof(numeroDeProdutos), 0);
    
    for (const auto& produto : produtos) {
        int nomeTamanho = produto.nome.size();
        send(clienteSocket, (char*)&nomeTamanho, sizeof(nomeTamanho), 0);
        send(clienteSocket, produto.nome.c_str(), nomeTamanho, 0);
        send(clienteSocket, (char*)&produto.precoPorKg, sizeof(produto.precoPorKg), 0);
        send(clienteSocket, (char*)&produto.quantidadeKg, sizeof(produto.quantidadeKg), 0);
        send(clienteSocket, (char*)&produto.precoPorUnidade, sizeof(produto.precoPorUnidade), 0);
        send(clienteSocket, (char*)&produto.quantidadeUnidade, sizeof(produto.quantidadeUnidade), 0);
    }
}

/**
 * @brief Processa a conexão com o cliente.
 * 
 * Após a conexão ser aceita, esta função chama `enviarProdutos` para
 * enviar a lista de produtos ao cliente.
 * 
 * @param clienteSocket O socket do cliente conectado.
 */
void processarConexao(SOCKET clienteSocket) {
    enviarProdutos(clienteSocket); 
}

/**
 * @brief Inicia o servidor e aguarda conexões.
 * 
 * O servidor é configurado e aguarda conexões de clientes. Quando um
 * cliente se conecta, a conexão é processada e a lista de produtos
 * é enviada.
 */
void iniciarServidor() {
    SOCKET servidorSocket = configurarServidor();
    if (servidorSocket == INVALID_SOCKET) {
        return; 
    }

    cout << "Aguardando conexoes...\n"; 

    SOCKET clienteSocket = aceitarConexao(servidorSocket);
    if (clienteSocket == INVALID_SOCKET) {
        closesocket(servidorSocket);
        WSACleanup();
        return; 
    }

    cout << "Cliente conectado. Processando conexao...\n"; 
    processarConexao(clienteSocket); 

    closesocket(clienteSocket);
    closesocket(servidorSocket);
    WSACleanup();
}