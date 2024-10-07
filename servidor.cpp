#include <iostream> 
#include <cstring> // Inclui a biblioteca para manipulação de strings e arrays
#include <winsock2.h> // Inclui a biblioteca para uso de sockets no Windows
#pragma comment(lib, "ws2_32.lib") // Indica ao linker para usar a biblioteca ws2_32.lib para Winsock
#define MAX_PRODUTOS 5 // Define o número máximo de produtos no estoque
#define PORTA 12345 // Define a porta que o servidor irá utilizar
using namespace std; 

struct ProdutoUnidade{
    char nome[50];
    float precoPorUnidade;
    float quantidadeUnidade;
};

// Estrutura para representar um produto
struct Produto {
    char nome[50]; // Nome do produto (até 50 caracteres)
    float precoPorKg; // Preço por quilograma do produto
    float quantidade; // Quantidade disponível do produto
};

// Inicializa o estoque de produtos com valores padrão
Produto produtos[MAX_PRODUTOS] = {
    {"Banana", 2.50, 100.0},
    {"Maçã", 3.00, 50.0},
    {"Laranja", 1.80, 75.0},
    {"Morango", 6.00, 30.0},
    {"Melancia", 12.00, 20.0}
};

ProdutoUnidade produtosunidade[MAX_PRODUTOS] = {
    {"Amora", 0.95, 100.0},
    {"Amendoim", 0.47, 50.0},
    {"Caju", 0.71, 75.0},
    {"Tomate", 0.82, 30.0},
    {"Uva", 0.37, 20.0}
};

Produto estoqueOriginal[MAX_PRODUTOS]; // Array para armazenar o estoque original
ProdutoUnidade estoqueOriginalUnidade[MAX_PRODUTOS];

// Função para carregar o estoque de um arquivo

void carregarEstoqueUnidade() {
    FILE *arquivounidade = fopen("estoqueunidade.txt", "r"); // Abre o arquivo estoqueunidade.txt em modo de leitura
    if (arquivounidade == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Arquivo de estoque não encontrado. Usando valores padrão.\n"; // Mensagem se o arquivo não existe
        return; // Retorna da função se o arquivo não foi encontrado
    }

    // Lê os produtos do arquivo
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        // Tenta ler os dados do produto do arquivo
        if (fscanf(arquivounidade, "%49[^-] - R$ %f por unidade - Quantidade: %f",
                   produtosunidade[i].nome, 
                   &produtosunidade[i].precoPorUnidade, 
                   &produtosunidade[i].quantidadeUnidade) != 3) {
            // Se houver um erro na leitura, define valores padrão
            cout << "Erro ao ler o arquivo de estoque. Usando valores padrão.\n";
            strcpy(produtosunidade[i].nome, "Produto Desconhecido"); // Define um nome padrão se falhar
            produtosunidade[i].precoPorUnidade = 0.0; // Preço padrão
            produtosunidade[i].quantidadeUnidade = 0.0; // Quantidade padrão
        }
    }

    memcpy(estoqueOriginalUnidade, produtosunidade, sizeof(produtosunidade)); // Copia o estoque atual para o estoque original
    fclose(arquivounidade); // Fecha o arquivo
}
void carregarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "r"); // Abre o arquivo estoque.txt em modo de leitura
    if (arquivo == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Arquivo de estoque não encontrado. Usando valores padrão.\n"; // Mensagem se o arquivo não existe
        return; // Retorna da função se o arquivo não foi encontrado
    }
    // Lê os produtos do arquivo
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        // Tenta ler os dados do produto do arquivo
        if (fscanf(arquivo, "%49[^-] - R$ %f por kg - Quantidade: %f kg\n", 
                   produtos[i].nome, 
                   &produtos[i].precoPorKg, 
                   &produtos[i].quantidade) != 3) {
            // Mensagem de erro se a leitura falhar
            cout << "Erro ao ler o arquivo de estoque. Usando valores padrão.\n";
            strcpy(produtos[i].nome, "Produto Desconhecido"); // Define um nome padrão se falhar
            produtos[i].precoPorKg = 0.0; // Preço padrão
            produtos[i].quantidade = 0.0; // Quantidade padrão
        }
    }
    memcpy(estoqueOriginal, produtos, sizeof(produtos)); // Copia o estoque atual para o original
    fclose(arquivo); // Fecha o arquivo
}


void salvarEstoqueUnidade() {
    FILE *arquivounidade = fopen("estoqueunidade.txt", "w");
    if (arquivounidade ==nullptr) {
        cout << "Erro ao abrir arquivo de estoque.\n";
        return;
    }
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        fprintf(arquivounidade, "%s - R$ %.2f por unidade - quantidade: %.2f unidade\n",
                produtosunidade[i].nome, produtosunidade[i].precoPorUnidade, produtosunidade[i].quantidadeUnidade);
    }
    fclose(arquivounidade);
}


// Função para salvar o estoque em um arquivo
void salvarEstoque() {
    FILE *arquivo = fopen("estoque.txt", "w"); // Abre o arquivo estoque.txt em modo de escrita
    if (arquivo == nullptr) { // Verifica se o arquivo foi aberto corretamente
        cout << "Erro ao abrir arquivo de estoque.\n"; // Mensagem de erro
        return; // Retorna da função se falhar
    }
    // Escreve os produtos no arquivo
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        fprintf(arquivo, "%s - R$ %.2f por kg - Quantidade: %.2f kg\n", 
                produtos[i].nome, produtos[i].precoPorKg, produtos[i].quantidade); // Formata e grava cada produto
    }
    fclose(arquivo); // Fecha o arquivo
}

void enviarProdutosUnidade(SOCKET clienteSocket){
    for (int i = 0; i < MAX_PRODUTOS; i++){
        cout << "Enviando produto" << i << ":" << produtosunidade[i].nome << " - " << produtosunidade[i].quantidadeUnidade << "unidade\n";

        if (send(clienteSocket, (char*)&produtosunidade[i], sizeof(ProdutoUnidade), 0) == SOCKET_ERROR){
            cout << "Erro ao enviar produto " << i + 1 << ".Código de erro: " << WSAGetLastError << "\n";
            return;
        }
    }
}


// Função para enviar a lista de produtos ao cliente
void enviarProdutos(SOCKET clienteSocket) {
    for (int i = 0; i < MAX_PRODUTOS; i++) { // Loop sobre todos os produtos
        cout << "Enviando produto " << i << ": " << produtos[i].nome << " - " << produtos[i].quantidade << " kg\n"; // Mensagem de envio
        // Envia os dados do produto para o cliente
        if (send(clienteSocket, (char*)&produtos[i], sizeof(Produto), 0) == SOCKET_ERROR) {
            cout << "Erro ao enviar produto " << i + 1 << ". Código de erro: " << WSAGetLastError() << "\n"; // Mensagem de erro
            return; // Retorna da função se falhar
        }
    }
}

void atualizarQuantidadeUnidade(int index, float quantidade){
    if (index >= 0 && index < MAX_PRODUTOS){
        cout << "Atualizando produto " << index << ": " << produtosunidade[index].nome << ", quantidade a subtrair: " << quantidade << "\n";
        produtosunidade[index].quantidadeUnidade -= quantidade; // Subtrai a quantidade do estoque
        salvarEstoqueUnidade();
    } else {
        cout << "Índice inválido: " << index << "\n";
    }
}


// Função para atualizar a quantidade do produto
void atualizarQuantidade(int index, float quantidade) {
    if (index >= 0 && index < MAX_PRODUTOS) { // Verifica se o índice é válido
        cout << "Atualizando produto " << index << ": " << produtos[index].nome << ", quantidade a subtrair: " << quantidade << "\n"; // Mensagem de atualização
        produtos[index].quantidade -= quantidade; // Subtrai a quantidade do estoque
        salvarEstoque(); // Salva as alterações no arquivo
    } else {
        cout << "Índice inválido: " << index << "\n"; // Mensagem de erro se o índice for inválido
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
    carregarEstoqueUnidade();
    carregarEstoque(); // Chama a função para carregar os produtos do arquivo
    
    // Inicia o servidor
    iniciarServidor(); // Chama a função para iniciar o servidor

    return 0; // Retorna 0 ao sistema, indicando que o programa terminou com sucesso
}