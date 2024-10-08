#include <iostream> 
#include <cstring> // Inclui funções para manipulação de strings
#include <winsock2.h> // Inclui a biblioteca Winsock para comunicação em rede no Windows
#include <ctime> // Inclui funções para manipulação de data e hora
#include <iomanip>
#include <fstream>
#include <cctype> // Inclui funções para verificar tipos de caracteres, como isdigit
#include <algorithm> // Inclui algoritmos da STL, como std::all_of
#include <vector>
#pragma comment(lib, "ws2_32.lib") // Especifica a biblioteca a ser ligada
#define PORTA 12345 // Define a porta para conexão
using namespace std; 

// Estrutura que representa um produto
struct Produto {
    string nome; // Nome do produto
    float precoPorKg; // Preço por quilo do produto
    float quantidade; // Quantidade disponível do produto
};

vector<Produto> produtos; // Usando vector para gerenciar a lista de produtos
vector<Produto> estoqueOriginal; // Estoque original para restauração

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
    cout << "Tentando conectar ao servidor...\n";
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

// Função para receber produtos do servidor
void receberProdutos(SOCKET clienteSocket) {
    int numeroDeProdutos;
    if (recv(clienteSocket, (char*)&numeroDeProdutos, sizeof(int), 0) == SOCKET_ERROR) {
        cout << "Erro ao receber dados do servidor. Codigo de erro: " << WSAGetLastError() << "\n";
        closesocket(clienteSocket);
        WSACleanup();
        exit(1);
    }

    for (int i = 0; i < numeroDeProdutos; i++) {
        Produto produto;

        // Recebe nome do produto
        int nomeTamanho;
        if (recv(clienteSocket, (char*)&nomeTamanho, sizeof(nomeTamanho), 0) == SOCKET_ERROR) {
            cout << "Erro ao receber o tamanho do nome do produto. Código de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }
       
        // Usando std::string para gerenciar a memória
        char* nomeBuffer = new char[nomeTamanho + 1];
        if (recv(clienteSocket, nomeBuffer, nomeTamanho, 0) == SOCKET_ERROR) {
            cout << "Erro ao receber o nome do produto. Código de erro: " << WSAGetLastError() << "\n";
            delete[] nomeBuffer;
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }
        nomeBuffer[nomeTamanho] = '\0'; // Adiciona o terminador de string
        produto.nome = string(nomeBuffer);
        delete[] nomeBuffer;

        // Recebe preço e quantidade
        if (recv(clienteSocket, (char*)&produto.precoPorKg, sizeof(produto.precoPorKg), 0) == SOCKET_ERROR ||
            recv(clienteSocket, (char*)&produto.quantidade, sizeof(produto.quantidade), 0) == SOCKET_ERROR) {
            cout << "Erro ao receber dados do servidor. Código de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }

        produtos.push_back(produto);
        estoqueOriginal.push_back(produto); // Copia para o estoque original
    }

    // Exibe os produtos
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < produtos.size(); i++) {
        cout << i + 1 << ". " << produtos[i].nome << " - R$ " << produtos[i].precoPorKg << " por kg (Quantidade disponível: " << produtos[i].quantidade << " kg)\n";
    }
}

// Função para exibir o menu de produtos
void exibirMenu() {
    limparTela();
    cout << "=====================================================================================================\n";
    cout << "                                     SELECIONE OS PRODUTOS                                           \n";
    cout << "=====================================================================================================\n";
    cout << fixed << setprecision(2);
    for (size_t i = 0; i < produtos.size(); i++) {
        cout << i + 1 << ". " << produtos[i].nome << " - R$ " << produtos[i].precoPorKg << " por kg (Quantidade disponível: " << produtos[i].quantidade << " kg)\n";
    }
    cout << "=====================================================================================================\n";
    cout << "Digite o numero do produto (ou 0 para cancelar): ";
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
    return (metodoPagamento == 1) ? (total * 0.10) : 0; // 10% de desconto para Pix
}

// Função para selecionar o método de pagamento
int selecionarMetodoPagamento() {
    limparTela(); // Limpa a tela
    int opcao; // Variável para armazenar a opção selecionada
    cout << "==============================================\n";
    cout << "              TIPO DE PAGAMENTO               \n";
    cout << "==============================================\n";
    cout << "1. Pix (10%% de desconto)\n";
    cout << "2. Cartão de Crédito (sem desconto)\n";
    cout << "==============================================\n";
    cout << "Escolha uma opcao: ";

    string verificaOpcao; // Variável para armazenar a entrada do usuário
    while (true) {  // Loop para validar a entrada
        cin >> verificaOpcao; // Lê a entrada
        if (isValidInteger(verificaOpcao)) { // Verifica se é um número inteiro
            opcao = stoi(verificaOpcao); // Converte para inteiro
            if (opcao == 1 || opcao == 2) break; // Aceita apenas opções 1 ou 2
        }
        cout << "Opcao invalida: "; // Mensagem de erro
    }
    return opcao; // Retorna a opção escolhida
}

void salvarEstoque() {
    ofstream arquivo("estoque.txt");
    if (!arquivo) {
        cout << "Erro ao abrir arquivo para salvar estoque.\n";
        return;
    }

    for (const auto& produto : produtos) {
        arquivo << produto.nome << " - R$ " << produto.precoPorKg << " por kg - Quantidade: " << produto.quantidade << " kg\n";
    }

    arquivo.close();
}

// Função para realizar a compra
void realizarCompra(SOCKET servidorSocket) {
    int escolha; // Variável para armazenar a escolha do produto
    float quantidade; // Variável para armazenar a quantidade do produto
    char continuar; // Variável para continuar ou não a compra
    vector<Produto> carrinho; // Usando vector para o carrinho
    float total = 0.0;

        limparTela(); // Limpa a tela

        while (true) { // Loop para adicionar produtos
            exibirMenu(); // Exibe o menu de produtos

            string verificaEscolha; // Variável para verificar a entrada do usuário
            while (true) { // Loop para validar a escolha do produto
                cin >> verificaEscolha; // Lê a entrada
                if (isValidInteger(verificaEscolha)) { // Verifica se é um número inteiro
                    escolha = stoi(verificaEscolha); // Converte para inteiro
                    if (escolha >= 0 && escolha <= produtos.size()) {
                    break; // Valor válido, sai do loop
                    }
                }
                cout << "Opcao invalida: "; // Mensagem de erro para entrada inválida
            }

            if (escolha == 0) { // Se o usuário escolher cancelar
                cout << "Compra cancelada. Restauração do estoque...\n";
            
                // Notifica o servidor sobre o cancelamento
                int cancelamento = -1; 
                if (send(servidorSocket, (char*)&cancelamento, sizeof(int), 0) == SOCKET_ERROR) {
                    cout << "Erro ao notificar cancelamento ao servidor. Código de erro: " << WSAGetLastError() << "\n";
                }

                // Restaura o estoque original e salva no arquivo
                produtos = estoqueOriginal; // Copia de volta o estoque original para produtos
                salvarEstoque(); // Salva o estoque restaurado no arquivo

                return; // Encerra o loop de compras e retorna ao menu principal
            }

        // Validação da quantidade do produto
        string verificaQuantidade; // Variável para verificar a quantidade
        while (true) { // Loop para validar a entrada da quantidade
            cout << "Informe o peso do produto (em kg): ";
            cin >> verificaQuantidade;

            // Substitui a vírgula por ponto
            replace(verificaQuantidade.begin(), verificaQuantidade.end(), ',', '.');

            try {
                quantidade = stof(verificaQuantidade); // Tenta converter para float
                if (quantidade > 0) {
                    break; // Peso válido
                }
            } catch (const invalid_argument&) {
                cout << "Opcao invalida "; // Mensagem de erro se a conversão falhar
            } catch (const out_of_range&) {
                cout << "Valor fora do intervalo! Tente novamente. "; // Tratamento para valores fora do intervalo
            }
        }

        if (quantidade > produtos[escolha - 1].quantidade) {
                cout << "Quantidade insuficiente! Tente novamente.\n";
                continue; // Retorna ao início do loop para escolher outro produto
            }

        if (send(servidorSocket, (char*)&escolha, sizeof(int), 0) == SOCKET_ERROR ||
            send(servidorSocket, (char*)&quantidade, sizeof(float), 0) == SOCKET_ERROR) {
            cout << "Erro ao enviar dados ao servidor. Código de erro: " << WSAGetLastError() << "\n";
            return;
        }

        produtos[escolha - 1].quantidade -= quantidade;
        total += produtos[escolha - 1].precoPorKg * quantidade;

         // Atualiza o carrinho
        bool produtoEncontrado = false;
        for (auto& item : carrinho) {
            if (item.nome == produtos[escolha - 1].nome) {
                item.quantidade += quantidade; // Atualiza a quantidade
                produtoEncontrado = true;
                break;
            }
        }

        if (!produtoEncontrado) {
            Produto novoProduto = produtos[escolha - 1];
            novoProduto.quantidade = quantidade;
            carrinho.push_back(novoProduto);
        }

        cout << "Você adicionou ao carrinho " << quantidade << " kg de " << produtos[escolha - 1].nome << "\n";

    while (true) {
    string verificaopcao;
    cout << "=====================================================================================================\n";
    cout << "1 - Deseja adicionar mais produtos  \n";
    cout << "2 - Finalizar compra                \n";
    cout << "=====================================================================================================\n";
    cout << "Escolha uma opcao: ";
    cin >> verificaopcao;

            if (isValidInteger(verificaopcao)) {
                int opcao = stoi(verificaopcao);
                if (opcao == 1) {
                    // Retorna ao loop de seleção de produtos
                    break; // Sai deste loop e recomeça a seleção de produtos
                } else if (opcao == 2) {
                    // Finalizar a compra
                    goto finalizaCompra; // Salta para a seção de finalização da compra
                }
            }
            cout << "Opcao inválida\n";
        }
    }

finalizaCompra:

    if (carrinho.empty()) {
        cout << "Nenhum item adicionado ao carrinho.\n";
        return;
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
     for (const auto& item : carrinho) {
        cout << item.nome << ": " << item.quantidade << " kg - R$ " << item.precoPorKg * item.quantidade << "\n";
    }
    cout << "*************************************\n";
    cout << "Desconto: R$ " << desconto << "\n"; // Exibe o desconto
    cout << "Total: R$ " << total << "\n"; // Exibe o total
    cout << "*************************************\n";
    cout << "AGRADECEMOS SUA COMPRA VOLTE SEMPRE!!\n"; // Mensagem de agradecimento
    cout << "*************************************\n";

    // Salva o estoque atualizado no arquivo
    salvarEstoque();

    cout << "Pressione qualquer tecla para voltar ao menu principal...\n"; // Mensagem para voltar ao menu
    cin.ignore(); // Limpa o buffer
    cin.get(); // Espera o usuário pressionar uma tecla

    // Limpa a tela antes de voltar ao menu principal
    limparTela();
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