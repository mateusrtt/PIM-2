#include <iostream> 
#include <cstring> 
#include <winsock2.h> 
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#pragma comment(lib, "ws2_32.lib") 
#define PORTA 12345 
using namespace std; 

// Estrutura para representar um produto
struct Produto {
    string nome; 
    float precoPorKg; 
    float precoPorUnidade; 
    float quantidadeKg; 
    int quantidadeUnidade; 
};

vector<Produto> produtos;  // Vetor para armazenar os produtos do arquivo
vector<Produto> estoqueOriginal;  // Cópia do estoque inicial

// Função para configurar o socket do servidor
SOCKET configurarServidor() {
    WSADATA wsaData; // Estrutura para armazenar informações da Winsock
    SOCKET servidorSocket; // Variável para o socket do servidor
    struct sockaddr_in servidorAddr; // Estrutura para armazenar informações do endereço do servidor

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // Inicializa a Winsock
        cout << "Falha ao inicializar Winsock. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
        return INVALID_SOCKET; // Retorna socket inválido em caso de erro
    }

    servidorSocket = socket(AF_INET, SOCK_STREAM, 0); // Cria um socket TCP
    if (servidorSocket == INVALID_SOCKET) { // Verifica se o socket foi criado corretamente
        cout << "Erro ao criar o socket. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
        WSACleanup(); // Limpa a Winsock
        return INVALID_SOCKET; // Retorna socket inválido em caso de erro
    }

    servidorAddr.sin_family = AF_INET; // Define a família do endereço como IPv4
    servidorAddr.sin_addr.s_addr = INADDR_ANY; // Define o endereço IP para aceitar conexões de qualquer interface
    servidorAddr.sin_port = htons(PORTA); // Define a porta do servidor (converte para a ordem correta)

    // Faz o bind do socket a um endereço e porta
    if (bind(servidorSocket, (struct sockaddr*)&servidorAddr, sizeof(servidorAddr)) == SOCKET_ERROR) {
        cout << "Erro ao fazer bind. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
        closesocket(servidorSocket); // Fecha o socket em caso de erro
        WSACleanup(); // Limpa a Winsock
        return INVALID_SOCKET; // Retorna socket inválido
    }

    // Configura o socket para ouvir conexões
    if (listen(servidorSocket, 1) == SOCKET_ERROR) {
        cout << "Erro ao ouvir conexões. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
        closesocket(servidorSocket); // Fecha o socket em caso de erro
        WSACleanup(); // Limpa a Winsock
        return INVALID_SOCKET; // Retorna socket inválido
    }
    return servidorSocket; // Retorna o socket do servidor configurado
}

// Função para aceitar a conexão do cliente
SOCKET aceitarConexao(SOCKET servidorSocket) {
    struct sockaddr_in clienteAddr; // Estrutura para armazenar informações do cliente
    int clienteAddrLen = sizeof(clienteAddr); // Tamanho da estrutura do cliente

    // Aceita a conexão de um cliente
    SOCKET clienteSocket = accept(servidorSocket, (struct sockaddr*)&clienteAddr, &clienteAddrLen);
    if (clienteSocket == INVALID_SOCKET) { // Verifica se a conexão foi aceita corretamente
        cout << "Erro ao aceitar conexão. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
    }
    return clienteSocket; // Retorna o socket do cliente
}

bool validarIndice(int index) {
    return index >= 0 && index < produtos.size();
}

// Função para atualizar a quantidade do produto
void atualizarQuantidade(int index, float quantidade, bool porUnidade) {
    if (!validarIndice(index)) {
         cout<<"Índice inválido: " + to_string(index);
        return;
    }

    if (porUnidade) {
        if (produtos[index].quantidadeUnidade >= quantidade) {
            produtos[index].quantidadeUnidade -= quantidade;
            cout<<"Produto " + produtos[index].nome + " atualizado: - " + to_string(quantidade) + " unidade(s).";
        } else {
            cout<<"Quantidade insuficiente para o produto: " + produtos[index].nome;
        }
    } else {
        if (produtos[index].quantidadeKg >= quantidade) {
            produtos[index].quantidadeKg -= quantidade;
            cout<<"Produto " + produtos[index].nome + " atualizado: - " + to_string(quantidade) + " kg.";
        } else {
            cout<<"Quantidade insuficiente para o produto: " + produtos[index].nome;
        }
    }
}

// Função para salvar o estoque em um arquivo
void salvarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "w"); // Abre o arquivo estoque.txt em modo de escrita
    if (arquivo == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Erro ao abrir arquivo de estoque.\n"; // Mensagem de erro
        return; // Retorna da função se falhar
    }
    for (const auto& produto : produtos) {
        fprintf(arquivo, "%s - R$ %.2f por kg - R$ %.2f por unidade - Quantidade: %.2f kg - Quantidade Unidades: %d\n",
                produto.nome.c_str(), produto.precoPorKg, produto.precoPorUnidade, produto.quantidadeKg, produto.quantidadeUnidade);
    }
    fclose(arquivo); // Fecha o arquivo
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
        cout<<"Operacao cancelada estoque restaurado";
    } else {
        salvarEstoque();
        cout<<"Compra realizada com sucesso";
    }
}

// Função para enviar a lista de produtos ao cliente
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

// Função para processar a conexão com o cliente
void processarConexao(SOCKET clienteSocket) {
    enviarProdutos(clienteSocket); 

    while (true) {
        int index;
        float quantidade; 
        bool porUnidade; 

        // Receber o índice do produto
        if (recv(clienteSocket, (char*)&index, sizeof(index), 0) <= 0) break; 
        index--;

        // Receber a quantidade do produto (por unidade)
        if (recv(clienteSocket, (char*)&quantidade, sizeof(quantidade), 0) <= 0) break;

        // Receber se a compra é por unidade
        if (recv(clienteSocket, (char*)&porUnidade, sizeof(porUnidade), 0) <= 0) break;

        processarMensagem(clienteSocket, index, quantidade, porUnidade);
    }
}

void carregarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "r"); // Abre o arquivo estoque.txt em modo de leitura
    if (arquivo == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Arquivo de estoque não encontrado\n";
        return;
    }

    produtos.clear();  // Limpa o vetor para recarregar
    Produto temp;
    char linhaBuffer[300];  // Buffer temporário para a linha

    while (fgets(linhaBuffer, sizeof(linhaBuffer), arquivo)) {
        linhaBuffer[strcspn(linhaBuffer, "\n")] = 0; 

        cout << "Lendo linha: " << linhaBuffer << endl;

        char nome[50]; // buffer temporário para o nome
        if (sscanf(linhaBuffer, "%49[^-] - R$ %f por kg - R$ %f por unidade - Quantidade: %f kg - Quantidade Unidades: %d",
            nome, &temp.precoPorKg, &temp.precoPorUnidade, &temp.quantidadeKg, &temp.quantidadeUnidade) == 5) {
            temp.nome = nome; // Copia o nome para o objeto Produto
            produtos.push_back(temp);
        } else {
            cout << "Falha ao analisar a linha: " << linhaBuffer << endl;
        }
    }

    estoqueOriginal = produtos;  // Atualiza o estoque original
    fclose(arquivo); // Fecha o arquivo
}

// Função principal do servidor que gerencia o loop de conexão
void iniciarServidor() {
    // Configura o servidor
    SOCKET servidorSocket = configurarServidor(); // Chama a função para configurar o servidor
    if (servidorSocket == INVALID_SOCKET) { // Verifica se a configuração falhou
        return; 
    }

    cout << "Aguardando conexões...\n"; 

    // Aceita conexão do cliente
    SOCKET clienteSocket = aceitarConexao(servidorSocket); // Aceita uma nova conexão
    if (clienteSocket == INVALID_SOCKET) { // Verifica se a conexão foi aceita corretamente
        closesocket(servidorSocket); // Fecha o socket do servidor
        WSACleanup(); // Limpa a Winsock
        return; 
    }

    cout << "Cliente conectado. Processando conexão...\n"; 
    // Processa a comunicação com o cliente
    processarConexao(clienteSocket); 

    // Fecha os sockets e limpa o Winsock
    closesocket(clienteSocket); // Fecha o socket do cliente
    closesocket(servidorSocket); // Fecha o socket do servidor
    WSACleanup(); // Limpa a Winsock
}

int main() {
    
    carregarEstoque(); 
    iniciarServidor(); 

    return 0;
}