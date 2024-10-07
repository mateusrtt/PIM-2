#include <iostream> 
#include <cstring> // Inclui funções para manipulação de strings
#include <winsock2.h> // Inclui a biblioteca Winsock para comunicação em rede no Windows
#include <ctime> // Inclui funções para manipulação de data e hora
#include <cctype> // Inclui funções para verificar tipos de caracteres, como isdigit
#include <algorithm> // Inclui algoritmos da STL, como std::all_of
#pragma comment(lib, "ws2_32.lib") // Especifica a biblioteca a ser ligada
#define MAX_PRODUTOS 5 // Define o número máximo de produtos
#define PORTA 12345 // Define a porta para conexão
using namespace std; 

int opcaoExibirMenu;

// Estrutura que representa um produto
struct Produto {
    char nome[50]; // Nome do produto
    float precoPorKg; // Preço por quilo do produto
    float quantidade; // Quantidade disponível do produto
};

struct ProdutosUnidade{
    char nome[50];
    float precoPorUnidade;
    float quantidadeUnidade;
};

// Array global para armazenar os produtos
Produto produtos[MAX_PRODUTOS];
ProdutosUnidade produtosunidade[MAX_PRODUTOS];

// Função para inicializar o socket
SOCKET inicializarSocket() {
    WSADATA wsaData; // Estrutura para armazenar dados de inicialização
    SOCKET clienteSocket; // Variável para o socket do cliente

    // Inicializa a Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Falha ao inicializar Winsock. Codigo de erro: " << WSAGetLastError() << "\n";
        exit(1); // Encerra o programa se falhar
    }

    // Cria um socket TCP
    clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        cout << "Erro ao criar o socket. Codigo de erro: " << WSAGetLastError() << "\n";
        WSACleanup(); // Limpa a Winsock
        exit(1); // Encerra o programa se falhar
    }

    return clienteSocket; // Retorna o socket criado
}

// Função para verificar se a entrada é um número inteiro válido
bool isValidInteger(const string& input) {
    return all_of(input.begin(), input.end(), ::isdigit); // Verifica se todos os caracteres são dígitos
}

// Função para conectar ao servidor
void conectarServidor(SOCKET clienteSocket, struct sockaddr_in& servidorAddr) {
    servidorAddr.sin_family = AF_INET; // Define a família de endereços
    servidorAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP do servidor (localhost)
    servidorAddr.sin_port = htons(PORTA); // Porta do servidor

    // Tenta conectar ao servidor
    if (connect(clienteSocket, (struct sockaddr*)&servidorAddr, sizeof(servidorAddr)) == SOCKET_ERROR) {
        cout << "Erro ao conectar ao servidor. Codigo de erro: " << WSAGetLastError() << "\n";
        closesocket(clienteSocket); // Fecha o socket em caso de erro
        WSACleanup(); // Limpa a Winsock
        exit(1); // Encerra o programa
    }

    cout << "Conectado ao servidor.\n"; // Mensagem de conexão bem-sucedida
}

// Função para limpar a tela do console
void limparTela() {
#ifdef _WIN32
    system("cls"); // Comando para Windows
#else
    system("clear"); // Comando para Unix/Linux
#endif
}


void receberProdutosUnidade(SOCKET clienteSocket){
    for (int i = 0; i < MAX_PRODUTOS; i++){
        if (recv(clienteSocket, (char*)&produtosunidade[i], sizeof(ProdutosUnidade), 0) == SOCKET_ERROR) {
            cout << "Erro ao receber dados do servidor. Código de erro" << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }
        cout << "Produto recebido" << i << ": " << produtosunidade[i].nome << " - R$ " << produtosunidade[i].precoPorUnidade << " por unidade, Quantidade: " << produtosunidade[i].quantidadeUnidade << " unidade\n";
    }
}
// Função para receber produtos do servidor
void receberProdutos(SOCKET clienteSocket) {
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        // Recebe dados do servidor e armazena em produtos[i]
        if (recv(clienteSocket, (char*)&produtos[i], sizeof(Produto), 0) == SOCKET_ERROR) {
            cout << "Erro ao receber dados do servidor. Codigo de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }
        // Exibe o produto recebido
        cout << "Produto recebido " << i << ": " << produtos[i].nome << " - R$ " << produtos[i].precoPorKg << " por kg, Quantidade: " << produtos[i].quantidade << " kg\n";
    }
}

void restaurarEstoqueUnidade(ProdutosUnidade* estoque) {
    FILE *arquivo = fopen("estoqueunidade.txt", "w");
    if (arquivo == nullptr){
        cout << "Erro ao abrir o arquivo de estoque.\n";
        return;
    }

    for (int i = 0; i < MAX_PRODUTOS; i++){
        fprintf(arquivo, "%s - R$ %.2f por unidade - Quantidade %.2f unidade\n",
                estoque[i]. nome, estoque[i].precoPorUnidade, estoque[i].quantidadeUnidade);
    }
    fclose(arquivo);
}


// Função para restaurar o estoque em um arquivo
void restaurarEstoque(Produto* estoque) {
    FILE *arquivo = fopen("estoque.txt", "w"); // Abre o arquivo para escrita
    if (arquivo == nullptr) {
        cout << "Erro ao abrir arquivo de estoque.\n";
        return; // Retorna se não conseguir abrir o arquivo
    }

    // Escreve os dados dos produtos no arquivo
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        fprintf(arquivo, "%s - R$ %.2f por kg - Quantidade: %.2f kg\n", 
                estoque[i].nome, estoque[i].precoPorKg, estoque[i].quantidade);
    }

    fclose(arquivo); // Fecha o arquivo
}



// Função para exibir o menu de produtos
void exibirMenu() {
    limparTela(); // Limpa a tela
    string verificaOpcao;
    cout << "==============================================================\n";
    //cout << "              Selecione os produtos comprados                 \n";
    cout << "              Selecione se será por unidade ou por peso       \n";
    cout << "==============================================================\n";
    cout << "1. Por unidade\n"; // Opção para comprar por unidade
    cout << "2. Por peso\n"; // Opção para comprar por peso
    cout << "==============================================================\n";
    cout << "Escolha uma opcao: ";
    while (true) {  // Loop para validar a entrada
        cin >> verificaOpcao; // Lê a entrada
        if (isValidInteger(verificaOpcao)) { // Verifica se é um número inteiro
            opcaoExibirMenu = stoi(verificaOpcao); // Converte para inteiro
            if (opcaoExibirMenu == 1 || opcaoExibirMenu == 2) break; // Aceita apenas opções 1 ou 2
        }
        cout << "Opcao invalida. Tente novamente: "; // Mensagem de erro
    }
    if (opcaoExibirMenu == 1) {
        for (int i = 0; i < MAX_PRODUTOS; i++) {
        cout << i + 1 << ". " << produtosunidade[i].nome << " - R$ " << produtosunidade[i].precoPorUnidade << 
        " por unidade (Quantidade disponivel: " << produtosunidade[i].quantidadeUnidade << " UN)\n";
    }
    } else if (opcaoExibirMenu == 2){
        for (int i = 0; i < MAX_PRODUTOS; i++) {
        cout << i + 1 << ". " << produtos[i].nome << " - R$ " << produtos[i].precoPorKg << 
        " por kg (Quantidade disponivel: " << produtos[i].quantidade << " kg)\n";
    }
    }
    // Exibe os produtos disponíveis
    
    cout << "===============================================================\n";
}

// Função para obter a data e hora formatada
void obterDataHora(char *buffer, size_t tamanho) {
    time_t t; // Variável para armazenar o tempo atual
    struct tm *tm_info; // Estrutura para armazenar informações da data/hora

    time(&t); // Obtém o tempo atual
    tm_info = localtime(&t); // Converte para a hora local
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M:%S", tm_info); // Formata a data/hora
}

// Função para calcular desconto
float calcularDesconto(float total, int metodoPagamento) {
    if (metodoPagamento == 1) { // Se for Pix
        return total * 0.10; // Retorna 10% de desconto
    }
    return 0; // Sem desconto para outros métodos
}

// Função para selecionar o método de pagamento
int selecionarMetodoPagamento() {
    limparTela(); // Limpa a tela
    int opcao; // Variável para armazenar a opção selecionada
    cout << "========================\n";
    cout << "    TIPO DE PAGAMENTO   \n";
    cout << "========================\n";
    cout << "1. Pix (10%% de desconto)\n";
    cout << "2. Cartão de Crédito (sem desconto)\n";
    cout << "========================\n";
    cout << "Escolha uma opcao: ";

    string verificaOpcao; // Variável para armazenar a entrada do usuário
    while (true) {  // Loop para validar a entrada
        cin >> verificaOpcao; // Lê a entrada
        if (isValidInteger(verificaOpcao)) { // Verifica se é um número inteiro
            opcao = stoi(verificaOpcao); // Converte para inteiro
            if (opcao == 1 || opcao == 2) break; // Aceita apenas opções 1 ou 2
        }
        cout << "Opcao invalida. Tente novamente: "; // Mensagem de erro
    }
    return opcao; // Retorna a opção escolhida
}

// Função para realizar a compra
void realizarCompra(SOCKET servidorSocket) {
    int escolha; // Variável para armazenar a escolha do produto
    float quantidade; // Variável para armazenar a quantidade do produto
    char continuar; // Variável para continuar ou não a compra
    Produto carrinho[MAX_PRODUTOS] = {0}; // Array para armazenar os produtos no carrinho
    ProdutosUnidade carrinhoUnidade[MAX_PRODUTOS] = {0}; // Array
    float total = 0.0; // Variável para armazenar o total da compra
    int carrinhoVazio = 1; // Flag para verificar se o carrinho está vazio
    Produto estoqueOriginal[MAX_PRODUTOS]; // Para armazenar o estoque original
    ProdutosUnidade estoqueOriginalUnidade[MAX_PRODUTOS];
    memcpy(estoqueOriginal, produtos, sizeof(produtos)); // Copia o estoque atual
    memcpy(estoqueOriginalUnidade, produtosunidade, sizeof(produtosunidade)); // Copia o estoque atual de volta

exibirMenu();
    if (opcaoExibirMenu == 1){


        limparTela(); // Limpa a tela

        while (true) { // Loop para adicionar produtos
            exibirMenu(); // Exibe o menu de produtos

            string verificaEscolha; // Variável para verificar a entrada do usuário
            while (true) { // Loop para validar a escolha do produto
                cout << "Digite o numero do produto que comprou (ou 0 para cancelar): ";
                cin >> verificaEscolha; // Lê a entrada
                if (isValidInteger(verificaEscolha)) { // Verifica se é um número inteiro
                    escolha = stoi(verificaEscolha); // Converte para inteiro
                                if (escolha >= 0 && escolha <= MAX_PRODUTOS) {
                    break; // Valor válido, sai do loop
                }
                }
                cout << "Entrada incorreta! "; // Mensagem de erro para entrada inválida
            }

            if (escolha == 0) { // Se o usuário escolher cancelar
                cout << "Compra cancelada.\n";
                // Restaura o estoque original
                memcpy(estoqueOriginalUnidade, produtosunidade, sizeof(produtosunidade)); // Copia o estoque atual de volta
                restaurarEstoqueUnidade(estoqueOriginalUnidade); 
                return; // Retorna da função
            }

        // Validação da quantidade do produto
        string verificaQuantidade; // Variável para verificar a quantidade
        while (true) { // Loop para validar a entrada da quantidade
            cout << "Informe a unidade do produto: ";
            cin >> verificaQuantidade; // Lê a quantidade
            try {
                quantidade = stof(verificaQuantidade); // Tenta converter para float
                if (quantidade > 0) {
                    break; // Peso válido, sai do loop
                }
            } catch (const invalid_argument& e) {
                // Se não for possível converter para float, uma exceção será lançada
            }
            cout << "Entrada incorreta!"; // Mensagem de erro para entrada inválida
        }

        // Verifica se a quantidade é suficiente
        if (quantidade > produtosunidade[escolha - 1].quantidadeUnidade) {
            cout << "Quantidade insuficiente! Tente novamente.\n"; // Mensagem de erro
            continue; // Continua para a próxima iteração do loop
        }

        // Envia a escolha do produto e a quantidade para o servidor
        if (send(servidorSocket, (char*)&escolha, sizeof(int), 0) == SOCKET_ERROR) {
            cout << "Erro ao enviar a escolha do produto. Codigo de erro: " << WSAGetLastError() << "\n";
            return; // Retorna em caso de erro
        }
        if (send(servidorSocket, (char*)&quantidade, sizeof(float), 0) == SOCKET_ERROR) {
            cout << "Erro ao enviar a quantidade. Codigo de erro: " << WSAGetLastError() << "\n";
            return; // Retorna em caso de erro
        }

        bool produtoEncontrado = false; // Flag para verificar se o produto já está no carrinho
        // Atualiza o carrinho com o produto selecionado
        for (int i = 0; i < MAX_PRODUTOS; i++) {
            if (strcmp(carrinhoUnidade[i].nome, produtosunidade[escolha - 1].nome) == 0) {
                carrinhoUnidade[i].quantidadeUnidade += quantidade; // Adiciona a quantidade ao produto já existente no carrinho
                produtoEncontrado = true; // Marca que o produto foi encontrado
                break;
            }
        }

        // Se o produto não estava no carrinho, adiciona-o
        if (!produtoEncontrado) {
            for (int i = 0; i < MAX_PRODUTOS; i++) {
                if (carrinhoUnidade[i].quantidadeUnidade == 0) { // Procura por um espaço vazio
                    strcpy(carrinhoUnidade[i].nome, produtosunidade[escolha - 1].nome); // Copia o nome do produto
                    carrinhoUnidade[i].precoPorUnidade = produtosunidade[escolha - 1].precoPorUnidade; // Define o preço
                    carrinhoUnidade[i].quantidadeUnidade = quantidade; // Define a quantidade
                    break; // Sai do loop
                }
            }
        }

        // Atualiza a quantidade de produtos disponíveis
        produtosunidade[escolha - 1].quantidadeUnidade -= quantidade;
        total += produtosunidade[escolha - 1].precoPorUnidade * quantidade; // Atualiza o total da compra

        carrinhoVazio = 0; // O carrinho não está vazio

        cout << "Você adicionou ao carrinho " << quantidade << " kg de " << produtosunidade[escolha - 1].nome << "\n";

        // Pergunta se o usuário deseja adicionar mais produtos
        cout << "Deseja adicionar outro produto na compra? (s/n): ";
        cin >> continuar; // Lê a resposta
        if (continuar == 'n' || continuar == 'N') {
            break; // Sai do loop se a resposta for não
        }
    }

    if (carrinhoVazio) {
        cout << "Nenhum item adicionado ao carrinho.\n"; // Mensagem se nada foi adicionado
        return; // Retorna da função
    }

    // Seleciona o método de pagamento
    int metodoPagamento = selecionarMetodoPagamento();
    float desconto = calcularDesconto(total, metodoPagamento); // Calcula o desconto
    total -= desconto; // Aplica o desconto ao total

    // Limpa a tela antes de mostrar o resumo da compra
    limparTela();

    // Obtém data e hora
    char dataHora[20]; // Declara um buffer para armazenar a data e hora
    obterDataHora(dataHora, sizeof(dataHora)); // Obtém a data e hora formatada

    // Exibe o resumo da compra
    cout << "**************************************\n";
    cout << "            NOTA FISCAL               \n";
    cout << "**************************************\n";
    cout << "Data e Hora: " << dataHora << "\n";
    cout << "**************************************\n";
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        if (carrinhoUnidade[i].quantidadeUnidade > 0) { // Se o produto foi adicionado
            cout << carrinhoUnidade[i].nome << ": " << carrinhoUnidade[i].quantidadeUnidade << " kg - R$ " << carrinhoUnidade[i].precoPorUnidade * carrinhoUnidade[i].quantidadeUnidade << "\n";
        }
    }
    cout << "*************************************\n";
    cout << "Desconto: R$ " << desconto << "\n"; // Exibe o desconto
    cout << "Total: R$ " << total << "\n"; // Exibe o total
    cout << "*************************************\n";
    cout << "AGRADECEMOS SUA COMPRA VOLTE SEMPRE!!\n"; // Mensagem de agradecimento
    cout << "*************************************\n";

    cout << "Pressione qualquer tecla para voltar ao menu principal...\n"; // Mensagem para voltar ao menu
    cin.ignore(); // Limpa o buffer
    cin.get(); // Espera o usuário pressionar uma tecla

    // Limpa a tela antes de voltar ao menu principal
    limparTela();
    }   
    
    
    
    else if (opcaoExibirMenu == 2){





        limparTela(); // Limpa a tela

    while (true) { // Loop para adicionar produtos
        exibirMenu(); // Exibe o menu de produtos

        string verificaEscolha; // Variável para verificar a entrada do usuário
        while (true) { // Loop para validar a escolha do produto
            cout << "Digite o numero do produto que comprou (ou 0 para cancelar): ";
            cin >> verificaEscolha; // Lê a entrada
            if (isValidInteger(verificaEscolha)) { // Verifica se é um número inteiro
                escolha = stoi(verificaEscolha); // Converte para inteiro
                                if (escolha >= 0 && escolha <= MAX_PRODUTOS) {
                    break; // Valor válido, sai do loop
                }
            }
            cout << "Entrada incorreta! "; // Mensagem de erro para entrada inválida
        }

        if (escolha == 0) { // Se o usuário escolher cancelar
            cout << "Compra cancelada.\n";
            // Restaura o estoque original
            memcpy(produtos, estoqueOriginal, sizeof(produtos)); // Copia o estoque original de volta
            restaurarEstoque(estoqueOriginal); // Restaura o estoque no arquivo
            return; // Retorna da função
        }

        // Validação da quantidade do produto
        string verificaQuantidade; // Variável para verificar a quantidade
        while (true) { // Loop para validar a entrada da quantidade
            cout << "Informe o peso do produto (em kg): ";
            cin >> verificaQuantidade; // Lê a quantidade
            try {
                quantidade = stof(verificaQuantidade); // Tenta converter para float
                if (quantidade > 0) {
                    break; // Peso válido, sai do loop
                }
            } catch (const invalid_argument& e) {
                // Se não for possível converter para float, uma exceção será lançada
            }
            cout << "Entrada incorreta!"; // Mensagem de erro para entrada inválida
        }

        // Verifica se a quantidade é suficiente
        if (quantidade > produtos[escolha - 1].quantidade) {
            cout << "Quantidade insuficiente! Tente novamente.\n"; // Mensagem de erro
            continue; // Continua para a próxima iteração do loop
        }

        // Envia a escolha do produto e a quantidade para o servidor
        if (send(servidorSocket, (char*)&escolha, sizeof(int), 0) == SOCKET_ERROR) {
            cout << "Erro ao enviar a escolha do produto. Codigo de erro: " << WSAGetLastError() << "\n";
            return; // Retorna em caso de erro
        }
        if (send(servidorSocket, (char*)&quantidade, sizeof(float), 0) == SOCKET_ERROR) {
            cout << "Erro ao enviar a quantidade. Codigo de erro: " << WSAGetLastError() << "\n";
            return; // Retorna em caso de erro
        }

        bool produtoEncontrado = false; // Flag para verificar se o produto já está no carrinho
        // Atualiza o carrinho com o produto selecionado
        for (int i = 0; i < MAX_PRODUTOS; i++) {
            if (strcmp(carrinho[i].nome, produtos[escolha - 1].nome) == 0) {
                carrinho[i].quantidade += quantidade; // Adiciona a quantidade ao produto já existente no carrinho
                produtoEncontrado = true; // Marca que o produto foi encontrado
                break;
            }
        }

        // Se o produto não estava no carrinho, adiciona-o
        if (!produtoEncontrado) {
            for (int i = 0; i < MAX_PRODUTOS; i++) {
                if (carrinho[i].quantidade == 0) { // Procura por um espaço vazio
                    strcpy(carrinho[i].nome, produtos[escolha - 1].nome); // Copia o nome do produto
                    carrinho[i].precoPorKg = produtos[escolha - 1].precoPorKg; // Define o preço
                    carrinho[i].quantidade = quantidade; // Define a quantidade
                    break; // Sai do loop
                }
            }
        }

        // Atualiza a quantidade de produtos disponíveis
        produtos[escolha - 1].quantidade -= quantidade;
        total += produtos[escolha - 1].precoPorKg * quantidade; // Atualiza o total da compra

        carrinhoVazio = 0; // O carrinho não está vazio

        cout << "Você adicionou ao carrinho " << quantidade << " kg de " << produtos[escolha - 1].nome << "\n";

        // Pergunta se o usuário deseja adicionar mais produtos
        cout << "Deseja adicionar outro produto na compra? (s/n): ";
        cin >> continuar; // Lê a resposta
        if (continuar == 'n' || continuar == 'N') {
            break; // Sai do loop se a resposta for não
        }
    }

    if (carrinhoVazio) {
        cout << "Nenhum item adicionado ao carrinho.\n"; // Mensagem se nada foi adicionado
        return; // Retorna da função
    }

    // Seleciona o método de pagamento
    int metodoPagamento = selecionarMetodoPagamento();
    float desconto = calcularDesconto(total, metodoPagamento); // Calcula o desconto
    total -= desconto; // Aplica o desconto ao total

    // Limpa a tela antes de mostrar o resumo da compra
    limparTela();

    // Obtém data e hora
    char dataHora[20]; // Declara um buffer para armazenar a data e hora
    obterDataHora(dataHora, sizeof(dataHora)); // Obtém a data e hora formatada

    // Exibe o resumo da compra
    cout << "**************************************\n";
    cout << "            NOTA FISCAL               \n";
    cout << "**************************************\n";
    cout << "Data e Hora: " << dataHora << "\n";
    cout << "**************************************\n";
    for (int i = 0; i < MAX_PRODUTOS; i++) {
        if (carrinho[i].quantidade > 0) { // Se o produto foi adicionado
            cout << carrinho[i].nome << ": " << carrinho[i].quantidade << " kg - R$ " << carrinho[i].precoPorKg * carrinho[i].quantidade << "\n";
        }
    }
    cout << "*************************************\n";
    cout << "Desconto: R$ " << desconto << "\n"; // Exibe o desconto
    cout << "Total: R$ " << total << "\n"; // Exibe o total
    cout << "*************************************\n";
    cout << "AGRADECEMOS SUA COMPRA VOLTE SEMPRE!!\n"; // Mensagem de agradecimento
    cout << "*************************************\n";

    cout << "Pressione qualquer tecla para voltar ao menu principal...\n"; // Mensagem para voltar ao menu
    cin.ignore(); // Limpa o buffer
    cin.get(); // Espera o usuário pressionar uma tecla

    // Limpa a tela antes de voltar ao menu principal
    limparTela();
    }

    
}

// Função que exibe o menu principal
void menuPrincipal(SOCKET clienteSocket) {
    while (true) {
        limparTela(); // Limpa a tela antes de exibir o menu
        cout << "========================\n";
        cout << "    CAIXA HORTIFRUTI    \n";
        cout << "========================\n";
        cout << "1. Fazer uma compra\n"; // Opção para fazer uma compra
        cout << "2. Cancelar compra\n"; // Opção para cancelar a compra
        cout << "========================\n";
        cout << "Escolha uma opcao: ";

        string verificaOpcao; // Variável para armazenar a entrada do usuário
        int opcao; // Variável para a opção escolhida

        // Valida a entrada do tipo de produto
        while (true) {
            cin >> verificaOpcao; // Lê a entrada do usuário
            if (isValidInteger(verificaOpcao)) { // Verifica se é um número inteiro
                opcao = stoi(verificaOpcao); // Converte para inteiro
                if (opcao == 1 || opcao == 2) break; // Aceita apenas opções 1 ou 2
            }
            cout << "Opcao invalida. Tente novamente: "; // Mensagem de erro
        }

        switch (opcao) { // Ação baseada na opção escolhida
            case 1:
                realizarCompra(clienteSocket); // Chama a função para realizar a compra
                break;
            case 2:
                cout << "Compra Cancelada...\n"; // Mensagem de cancelamento
                closesocket(clienteSocket); // Fecha o socket
                WSACleanup(); // Limpa a Winsock
                return; // Retorna da função
        }
    }
}



// Função principal do programa
int main() {
    struct sockaddr_in servidorAddr; // Estrutura para armazenar informações do servidor
    
    SOCKET clienteSocket = inicializarSocket(); // Inicializa o socket do cliente
    conectarServidor(clienteSocket, servidorAddr); // Conecta ao servidor
    receberProdutos(clienteSocket); // Recebe a lista de produtos do servidor
    
    // Chamando a função de menu principal
    menuPrincipal(clienteSocket); // Inicia o menu principal

    return 0; // Retorna 0, indicando que o programa terminou com sucesso
}