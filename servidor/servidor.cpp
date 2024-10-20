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

void salvarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "w");
    if (arquivo == nullptr) {
        cout << "Erro ao abrir arquivo de estoque.\n";
        return;
    }
    for (const auto& produto : produtos) {
        fprintf(arquivo, "%s - R$ %.2f por kg - R$ %.2f por unidade - Quantidade: %.2f kg - Quantidade Unidades: %d\n",
                produto.nome.c_str(), produto.precoPorKg, produto.precoPorUnidade, produto.quantidadeKg, produto.quantidadeUnidade);
    }
    fclose(arquivo);
}

bool validarIndice(int index) {
    return index >= 0 && index < produtos.size();
}

void atualizarQuantidade(int index, float quantidade, bool porUnidade) {
    if (!validarIndice(index)) {
        cout << "Índice inválido: " + to_string(index);
        return;
    }
    if (porUnidade) {
        if (produtos[index].quantidadeUnidade >= quantidade) {
            produtos[index].quantidadeUnidade -= quantidade;
            cout << "Produto " + produtos[index].nome + " atualizado: - " + to_string(quantidade) + " unidade(s).";
        } else {
            cout << "Quantidade insuficiente para o produto: " + produtos[index].nome;
        }
    } else {
        if (produtos[index].quantidadeKg >= quantidade) {
            produtos[index].quantidadeKg -= quantidade;
            cout << "Produto " + produtos[index].nome + " atualizado: - " + to_string(quantidade) + " kg.";
        } else {
            cout << "Quantidade insuficiente para o produto: " + produtos[index].nome;
        }
    }
}

void processarMensagem(SOCKET clienteSocket, int index, float quantidade, bool porUnidade) {
    if (index == -1) {
        return; // Encerra a conexão
    }

    float quantidadeAnteriorKg = produtos[index].quantidadeKg;
    int quantidadeAnteriorUnidade = produtos[index].quantidadeUnidade;

    atualizarQuantidade(index, quantidade, porUnidade);

    if (produtos[index].quantidadeKg == quantidadeAnteriorKg && produtos[index].quantidadeUnidade == quantidadeAnteriorUnidade) {
        produtos[index].quantidadeKg = quantidadeAnteriorKg;
        produtos[index].quantidadeUnidade = quantidadeAnteriorUnidade;
        salvarEstoque();
        cout << "Operação cancelada. Estoque restaurado.";
    } else {
        salvarEstoque();
        cout << "Compra realizada com sucesso.";
    }
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

    while (true) {
        int index;
        float quantidade; 
        bool porUnidade; 

        if (recv(clienteSocket, (char*)&index, sizeof(index), 0) <= 0) break; 
        index--;

        if (recv(clienteSocket, (char*)&quantidade, sizeof(quantidade), 0) <= 0) break;

        if (recv(clienteSocket, (char*)&porUnidade, sizeof(porUnidade), 0) <= 0) break;

        processarMensagem(clienteSocket, index, quantidade, porUnidade);
    }
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