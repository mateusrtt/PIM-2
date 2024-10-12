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

// Declaração do vetor de produtos
vector<Produto> produtos;  // Vetor para armazenar os produtos do arquivo
vector<Produto> estoqueOriginal;  // Cópia do estoque inicial

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
        send(clienteSocket, (char*)&produto.quantidadeKg, sizeof(produto.quantidadeKg), 0);
        send(clienteSocket, (char*)&produto.precoPorUnidade, sizeof(produto.precoPorUnidade), 0);
        send(clienteSocket, (char*)&produto.quantidadeUnidade, sizeof(produto.quantidadeUnidade), 0);
    }
}

// Função para atualizar a quantidade do produto
void atualizarQuantidade(int index, float quantidade, bool porUnidade) {
    if (index >= 0 && index < produtos.size()) {
              if (porUnidade) {
            if (produtos[index].quantidadeUnidade >= quantidade) {
                produtos[index].quantidadeUnidade -= quantidade;
                cout << "Produto " << produtos[index].nome << " atualizado: - " << quantidade << " unidade(s).\n";
            } else {
                cout << "Quantidade insuficiente para o produto: " << produtos[index].nome << "\n";
            }
        } else {
            if (produtos[index].quantidadeKg >= quantidade) {
                produtos[index].quantidadeKg -= quantidade;
                cout << "Produto " << produtos[index].nome << " atualizado: - " << quantidade << " kg.\n";
            } else {
                cout << "Quantidade insuficiente para o produto: " << produtos[index].nome << "\n";
            }
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
    int index;
    float quantidade; 
    int bytesRecebidos; 
    bool porUnidade; 

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

         // Receber a quantidade do produto (por unidade)
        bytesRecebidos = recv(clienteSocket, (char*)&quantidade, sizeof(quantidade), 0);
        if (bytesRecebidos == SOCKET_ERROR) {
            cout << "Erro ao receber quantidade. Código de erro: " << WSAGetLastError() << "\n";
            break; 
        } else if (bytesRecebidos == 0) {
            cout << "Conexão encerrada pelo cliente.\n";
            break; 
        }

        // Receber se a compra é por unidade
        bytesRecebidos = recv(clienteSocket, (char*)&porUnidade, sizeof(porUnidade), 0);
        if (bytesRecebidos == SOCKET_ERROR) {
            cout << "Erro ao receber informação de tipo de compra. Código de erro: " << WSAGetLastError() << "\n";
            break; 
        } else if (bytesRecebidos == 0) {
            cout << "Conexão encerrada pelo cliente.\n";
            break; 
        }

        if (index == -1) { // Se o índice for -1, encerra a conexão
            break; 
        }

        // Armazena o estado anterior
        float quantidadeAnteriorKg = produtos[index].quantidadeKg;
        int quantidadeAnteriorUnidade = produtos[index].quantidadeUnidade; // Manter como int

        // Tenta atualizar a quantidade do produto
        atualizarQuantidade(index, (float)quantidade, porUnidade); // Chama a função sem esperar retorno, convertendo quantidade para float se necessário

        // Verifica se a atualização foi bem-sucedida por meio de mensagens já impressas
        if (produtos[index].quantidadeKg == quantidadeAnteriorKg && produtos[index].quantidadeUnidade == quantidadeAnteriorUnidade) {
            // Restaura o estoque se houve falha
            produtos[index].quantidadeKg = quantidadeAnteriorKg;
            produtos[index].quantidadeUnidade = quantidadeAnteriorUnidade;
            salvarEstoque(); // Salva o estoque restaurado
            cout << "Operação cancelada. Estoque restaurado.\n";
        } else {
            salvarEstoque(); // Salva o estoque atualizado
            cout << "Compra realizada com sucesso!\n";
        }
    }
}

// Função principal do servidor que gerencia o loop de conexão
void iniciarServidor() {
    // Configura o servidor
    SOCKET servidorSocket = configurarServidor(); // Chama a função para configurar o servidor
    if (servidorSocket == INVALID_SOCKET) { // Verifica se a configuração falhou
        return; // Sai da função se houve erro
    }

    cout << "Aguardando conexões...\n"; 

    // Aceita conexão do cliente
    SOCKET clienteSocket = aceitarConexao(servidorSocket); // Aceita uma nova conexão
    if (clienteSocket == INVALID_SOCKET) { // Verifica se a conexão foi aceita corretamente
        closesocket(servidorSocket); // Fecha o socket do servidor
        WSACleanup(); // Limpa a Winsock
        return; // Sai da função
    }

    cout << "Cliente conectado. Processando conexão...\n"; 
    // Processa a comunicação com o cliente
    processarConexao(clienteSocket); // Chama a função para processar a conexão

    // Fecha os sockets e limpa o Winsock
    closesocket(clienteSocket); // Fecha o socket do cliente
    closesocket(servidorSocket); // Fecha o socket do servidor
    WSACleanup(); // Limpa a Winsock
}

int main() {
    
    carregarEstoque(); // Chama a função para carregar os produtos do arquivo
    iniciarServidor(); // Chama a função para iniciar o servidor

    return 0;
}