#include "servidor.h"
#include "sockets.h"

vector<Produto> produtos;
vector<Produto> estoqueOriginal;

void carregarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "r");
    if (arquivo == nullptr) {
        cout << "Arquivo de estoque não encontrado\n";
        return;
    }

    produtos.clear();
    Produto temp;
    char linhaBuffer[300];

    while (fgets(linhaBuffer, sizeof(linhaBuffer), arquivo)) {
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
    estoqueOriginal = produtos;
    fclose(arquivo);
}

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

void processarConexao(SOCKET clienteSocket) {
    enviarProdutos(clienteSocket); 
}

void iniciarServidor() {
    SOCKET servidorSocket = configurarServidor();
    if (servidorSocket == INVALID_SOCKET) {
        return; 
    }

    cout << "Aguardando conexões...\n"; 

    SOCKET clienteSocket = aceitarConexao(servidorSocket);
    if (clienteSocket == INVALID_SOCKET) {
        closesocket(servidorSocket);
        WSACleanup();
        return; 
    }

    cout << "Cliente conectado. Processando conexão...\n"; 
    processarConexao(clienteSocket); 

    closesocket(clienteSocket);
    closesocket(servidorSocket);
    WSACleanup();
}