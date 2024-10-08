#include <iostream> 
#include <cstring> // Inclui a biblioteca para manipulação de strings e arrays
#include <winsock2.h> // Inclui a biblioteca para uso de sockets no Windows
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#pragma comment(lib, "ws2_32.lib") // Indica ao linker para usar a biblioteca ws2_32.lib para Winsock
#define PORTA 12345 // Define a porta que o servidor irá utilizar
using namespace std; 

// Estrutura para representar um produto
struct Produto {
    string nome; // Nome do produto (até 50 caracteres)
    float precoPorKg; // Preço por quilograma do produto
    float quantidade; // Quantidade disponível do produto
};

// Declaração do vetor de produtos
vector<Produto> produtos;  // Vetor para armazenar os produtos do arquivo
vector<Produto> estoqueOriginal;  // Cópia do estoque inicial

void carregarProdutos(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return;
    }

    string linha;
    while (getline(file, linha)) {
        Produto produto;
        istringstream iss(linha);
        string precoStr, quantidadeStr;

        // Ignora o número do produto
        getline(iss, produto.nome, '-');
        iss >> precoStr; // Pega "R$" e o preço
        iss >> produto.precoPorKg; // Lê o preço
        iss.ignore(256, ' '); // Ignora "por kg"
        iss >> quantidadeStr; // Lê "Quantidade" e o valor
        iss >> produto.quantidade;

        produtos.push_back(produto);
    }
    file.close();
}

void carregarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "r"); // Abre o arquivo estoque.txt em modo de leitura
    if (arquivo == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Arquivo de estoque não encontrado\n";
        return;
    }

    produtos.clear();  // Limpa o vetor para recarregar
    Produto temp;
    char linhaBuffer[200];  // Buffer temporário para a linha

    while (fgets(linhaBuffer, sizeof(linhaBuffer), arquivo)) {
        linhaBuffer[strcspn(linhaBuffer, "\n")] = 0; 

        cout << "Lendo linha: " << linhaBuffer << endl;

        char nome[50]; // buffer temporário para o nome
        if (sscanf(linhaBuffer, "%49[^-] - R$ %f por kg - Quantidade: %f kg", 
                nome, &temp.precoPorKg, &temp.quantidade) == 3) {
                temp.nome = nome; // Copia o nome para o objeto Produto
            produtos.push_back(temp);
        } else {
            cout << "Falha ao analisar a linha: " << linhaBuffer << endl;
        }
    }

    estoqueOriginal = produtos;  // Atualiza o estoque original
    fclose(arquivo); // Fecha o arquivo
}

// Função para salvar o estoque em um arquivo
void salvarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "w"); // Abre o arquivo estoque.txt em modo de escrita
    if (arquivo == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Erro ao abrir arquivo de estoque.\n"; // Mensagem de erro
        return; // Retorna da função se falhar
    }
    for (const auto& produto : produtos) {
        fprintf(arquivo, "%s - R$ %.2f por kg - Quantidade: %.2f kg\n", 
                produto.nome.c_str(), produto.precoPorKg, produto.quantidade);
    }
    fclose(arquivo); // Fecha o arquivo
}

// Função para enviar a lista de produtos ao cliente
void enviarProdutos(SOCKET clienteSocket) {
    if (produtos.empty()) {
        cout << "Nenhum produto disponível para enviar.\n";
        int numeroDeProdutos = 0;
        send(clienteSocket, (char*)&numeroDeProdutos, sizeof(numeroDeProdutos), 0);
        return;
    }

    int numeroDeProdutos = produtos.size();
    send(clienteSocket, (char*)&numeroDeProdutos, sizeof(numeroDeProdutos), 0);
    
    for (const auto& produto : produtos) {
        int nomeTamanho = produto.nome.size();
        send(clienteSocket, (char*)&nomeTamanho, sizeof(nomeTamanho), 0);
        send(clienteSocket, produto.nome.c_str(), nomeTamanho, 0);
        send(clienteSocket, (char*)&produto.precoPorKg, sizeof(produto.precoPorKg), 0);
        send(clienteSocket, (char*)&produto.quantidade, sizeof(produto.quantidade), 0);
    }
}

// Função para atualizar a quantidade do produto
void atualizarQuantidade(int index, float quantidade) {
    if (index >= 0 && index < produtos.size()) {
        if (produtos[index].quantidade >= quantidade) {
            cout << "Atualizando produto " << index << ": " << produtos[index].nome << ", quantidade a subtrair: " << quantidade << "\n";
            produtos[index].quantidade -= quantidade;
            salvarEstoque();
        } else {
            cout << "Quantidade insuficiente para o produto: " << produtos[index].nome << "\n";
        }
    } else {
        cout << "Índice inválido: " << index << "\n";
    }
}

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

// Função para processar a conexão com o cliente
void processarConexao(SOCKET clienteSocket) {
    int index; // Variável para armazenar o índice do produto
    float quantidade; // Variável para armazenar a quantidade do produto
    int bytesRecebidos; // Variável para armazenar a quantidade de bytes recebidos

    enviarProdutos(clienteSocket); // Envia a lista de produtos para o cliente

    while (true) { // Loop para processar as mensagens do cliente
        // Receber o índice do produto
        bytesRecebidos = recv(clienteSocket, (char*)&index, sizeof(index), 0); // Recebe o índice do produto
        if (bytesRecebidos == SOCKET_ERROR) { // Verifica se ocorreu erro na recepção
            cout << "Erro ao receber índice do produto. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
            break; // Sai do loop em caso de erro
        } else if (bytesRecebidos == 0) { // Verifica se a conexão foi encerrada
            cout << "Conexão encerrada pelo cliente.\n"; // Mensagem de encerramento
            break; // Sai do loop
        }

        index--;  // Ajusta o índice para o array (começando em 0)

        // Receber a quantidade do produto
        bytesRecebidos = recv(clienteSocket, (char*)&quantidade, sizeof(quantidade), 0); // Recebe a quantidade do produto
        if (bytesRecebidos == SOCKET_ERROR) { // Verifica se ocorreu erro na recepção
            cout << "Erro ao receber quantidade. Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
            break; // Sai do loop em caso de erro
        } else if (bytesRecebidos == 0) { // Verifica se a conexão foi encerrada
            cout << "Conexão encerrada pelo cliente.\n"; // Mensagem de encerramento
            break; // Sai do loop
        }

        if (index == -1) { // Se o índice for -1, encerra a conexão
            break; // Termina a conexão
        }

        atualizarQuantidade(index, quantidade); // Atualiza a quantidade do produto
    }
}

// Função principal do servidor que gerencia o loop de conexão
void iniciarServidor() {
    // Configura o servidor
    SOCKET servidorSocket = configurarServidor(); // Chama a função para configurar o servidor
    if (servidorSocket == INVALID_SOCKET) { // Verifica se a configuração falhou
        return; // Sai da função se houve erro
    }

    cout << "Aguardando conexões...\n"; // Mensagem aguardando conexão

    // Aceita conexão do cliente
    SOCKET clienteSocket = aceitarConexao(servidorSocket); // Aceita uma nova conexão
    if (clienteSocket == INVALID_SOCKET) { // Verifica se a conexão foi aceita corretamente
        closesocket(servidorSocket); // Fecha o socket do servidor
        WSACleanup(); // Limpa a Winsock
        return; // Sai da função
    }

    cout << "Cliente conectado. Processando conexão...\n"; // Mensagem de cliente conectado

    // Processa a comunicação com o cliente
    processarConexao(clienteSocket); // Chama a função para processar a conexão

    // Fecha os sockets e limpa o Winsock
    closesocket(clienteSocket); // Fecha o socket do cliente
    closesocket(servidorSocket); // Fecha o socket do servidor
    WSACleanup(); // Limpa a Winsock
}

// Função principal do programa
int main() {
    // Carrega o estoque inicial
    carregarEstoque(); // Chama a função para carregar os produtos do arquivo
    
    // Inicia o servidor
    iniciarServidor(); // Chama a função para iniciar o servidor

    return 0; // Retorna 0 ao sistema, indicando que o programa terminou com sucesso
}