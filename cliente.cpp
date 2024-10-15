#include <iostream> 
#include <cstring> 
#include <winsock2.h> 
#include <ctime> 
#include <iomanip>
#include <fstream>
#include <cctype> 
#include <algorithm> 
#include <vector>
#pragma comment(lib, "ws2_32.lib") 
#define PORTA 12345 
using namespace std; 

// Estrutura que representa um produto
struct Produto {
    string nome; 
    float precoPorKg; 
    float precoPorUnidade; 
    float quantidadeKg; 
    int quantidadeUnidade;
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
        exit(1); 
    }
    // Cria um socket TCP
    clienteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clienteSocket == INVALID_SOCKET) {
        cout << "Erro ao criar o socket. Codigo de erro: " << WSAGetLastError() << "\n";
        WSACleanup(); 
        exit(1); 
    }
    return clienteSocket; // Retorna o socket criado
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
            recv(clienteSocket, (char*)&produto.quantidadeKg, sizeof(produto.quantidadeKg), 0) == SOCKET_ERROR ||
            recv(clienteSocket, (char*)&produto.precoPorUnidade, sizeof(produto.precoPorUnidade), 0) == SOCKET_ERROR ||
            recv(clienteSocket, (char*)&produto.quantidadeUnidade, sizeof(int), 0) == SOCKET_ERROR) {
            cout << "Erro ao receber dados do servidor. Código de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }
        produtos.push_back(produto);
        estoqueOriginal.push_back(produto); // Copia para o estoque original
    }
}

// Função para obter a data e hora formatada
void obterDataHora(char *buffer, size_t tamanho) {
    time_t t; // Variável para armazenar o tempo atual
    struct tm *tm_info; // Estrutura para armazenar informações da data/hora

    time(&t); // Obtém o tempo atual
    tm_info = localtime(&t); // Converte para a hora local
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M:%S", tm_info); // Formata a data/hora
}

bool isValidInteger(const string& input) {
    if (input.empty()) return false; // Verifica se a entrada não está vazia
    return all_of(input.begin(), input.end(), ::isdigit); // Verifica se todos os caracteres são dígitos
}

bool stringValida(const string& entrada) {
    return !entrada.empty() && all_of(entrada.begin(), entrada.end(), [](unsigned char c) {
        return isalpha(c) || isspace(c);
    });
}

bool isValidFloat(const string& str) {
    istringstream iss(str);
    float f;
    return iss >> f && iss.eof();
}

void mudaCor(int corTexto, int corFundo=0) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, corTexto | (corFundo << 4));
}

// Função para limpar a tela do console
void limparTela() {
#ifdef _WIN32
    system("cls"); // Comando para Windows
#else
    system("clear"); // Comando para Unix/Linux
#endif
}

void salvarEstoque() {
    ofstream arquivo("estoque.txt");
    if (!arquivo) {
        cout << "Erro ao abrir arquivo para salvar estoque.\n";
        return;
    }

    for (const auto& produto : produtos) {
        arquivo << produto.nome << " - R$ " << produto.precoPorKg << " por kg - "
                << "R$ " << produto.precoPorUnidade << " por unidade - "
                << "Quantidade: " << produto.quantidadeKg << " kg - "
                << "Quantidade Unidades: " << produto.quantidadeUnidade << endl;
    }

    arquivo.close();
}

int obterOpcaoValida(int min, int max) {
    string verificaOpcao;
    int opcao;

    while (true) {  // Loop até obter uma entrada válida
        cin >> verificaOpcao; // Lê a entrada
        if (isValidInteger(verificaOpcao)) { // Verifica se é um número inteiro
            opcao = stoi(verificaOpcao); // Converte para inteiro
            if (opcao >= min && opcao <= max) {
                return opcao; // Retorna a opção válida
            }
        }
        cout << "Opcao invalida: "; // Mensagem de erro
    }
}

void calcularDesconto(float total, int opcao, float& desconto, float& totalComDesconto, string& tipoPagamento) {
    desconto = 0.0; // Inicializa o desconto
    totalComDesconto = total; // Inicializa o total após desconto
    // Calcula o desconto com base na opção escolhida
    if (opcao == 1) {
        desconto = total * 0.10; // 10% de desconto
        tipoPagamento = "Dinheiro";
    } else if (opcao == 2) {
        desconto = total * 0.05; // 5% de desconto
        tipoPagamento = "Pix";
    } else {
        tipoPagamento = "Cartão de Crédito"; // Sem desconto
    }
    totalComDesconto -= desconto; // Atualiza o total com desconto
}

// Função para exibir o menu de produtos
void exibirMenu() {
    limparTela();
    mudaCor(1); 
    cout << "=========================================================================================================\n";
    mudaCor(1,6);
    cout << "                                         SELECIONE OS PRODUTOS                                           \n";
     mudaCor(1); 
    cout << "=========================================================================================================\n";
    cout << left << setw(5) << "ID" 
         << setw(20) << "Produto" 
         << setw(20) << "Preco kg" 
         << setw(20) << "Preco unid" 
         << setw(20) << "Qtd.kg" 
         << setw(20) << "Qtd.unid" << endl;
    cout << "=========================================================================================================\n";
    mudaCor(7,1); 
    for (size_t i = 0; i < produtos.size(); i++) {
        string nomeProduto = produtos[i].nome;
        if (nomeProduto.length() > 20) {
            nomeProduto = nomeProduto.substr(0, 17); 
        }
        cout << left << setw(5) << (i + 1) 
             << setw(20) << nomeProduto 
             << setw(20) << fixed << setprecision(2) << produtos[i].precoPorKg 
             << setw(20) << fixed << setprecision(2) << produtos[i].precoPorUnidade 
             << setw(20) << fixed << setprecision(2) << produtos[i].quantidadeKg 
             << setw(20) << produtos[i].quantidadeUnidade << endl;
    }
    mudaCor(1); 
    cout << "=========================================================================================================\n";
    cout << "Digite o numero do produto: ";
}

int selecionaProduto(){
    exibirMenu();
    int opcao = obterOpcaoValida(1, produtos.size()); 
}

void cancelarCompra(vector<Produto>& estoqueOriginal){
    produtos = estoqueOriginal; // Restaura o estoque original
    salvarEstoque(); 
}

int opcaoCompra(){
    cout << "=========================================================================================================\n";
    mudaCor(1, 6);
    cout << "                                           FORMA DE COMPRA                                                \n";
    mudaCor(1);
    cout << "=========================================================================================================\n";
    cout << "1. Por kg\n";
    cout << "2. Por unidade\n";
    cout << "=========================================================================================================\n";
    cout << "Escolha uma opcao: ";
    int opcao = obterOpcaoValida(1, 2);
}

bool validarQuantidade(int formaCompra, const string& verificaQuantidade, float& quantidade, int escolha) {
    // Verifica se a entrada é válida de acordo com a forma de compra
    if (formaCompra == 1) { // Compra em kg
        if (isValidFloat(verificaQuantidade)) {
            quantidade = stof(verificaQuantidade); // Converte para float
            if (quantidade > 0 && quantidade <= produtos[escolha - 1].quantidadeKg) {
                produtos[escolha - 1].quantidadeKg -= quantidade; // Atualiza a quantidade em kg
                return true;
            }
        }
    } else { // Compra em unidades
        if (isValidInteger(verificaQuantidade)) {
            quantidade = stoi(verificaQuantidade); // Converte para int
            if (quantidade > 0 && quantidade <= produtos[escolha - 1].quantidadeUnidade) {
                produtos[escolha - 1].quantidadeUnidade -= quantidade; // Atualiza a quantidade em unidades
                return true;
            }
        }
    }
    return false; // Se não passar nas validações
}

float obterQuantidade(int formaCompra, int escolha) {
    string verificaQuantidade; // Variável para verificar a quantidade
    float quantidade = 0.0;

    while (true) {
        if (formaCompra == 1) {
            cout << "Informe o peso do produto (em kg): ";
        } else {
            cout << "Informe a quantidade do produto (em unidades): ";
        }
        cin >> verificaQuantidade;

        // Substitui vírgula por ponto para permitir entrada decimal
        replace(verificaQuantidade.begin(), verificaQuantidade.end(), ',', '.');

        // Chama a função de validação
        if (validarQuantidade(formaCompra, verificaQuantidade, quantidade, escolha)) {
            return quantidade; // Retorna a quantidade válida
        }

        cout << "Opcao invalida: \n"; 
    }
}

void atualizarCarrinho(vector<Produto>& carrinho, int formaCompra, float quantidade, int escolha, float& total) {
    if (formaCompra == 1) { // Comprando por kg
        total += produtos[escolha - 1].precoPorKg * quantidade;
    } else { // Comprando por unidade
        total += produtos[escolha - 1].precoPorUnidade * quantidade;
    }

    salvarEstoque();

    // Atualiza o carrinho
    bool produtoEncontrado = false;
    for (auto& item : carrinho) {
        if (item.nome == produtos[escolha - 1].nome) {
            if (formaCompra == 1) {
                item.quantidadeKg += quantidade; // Atualiza a quantidade em kg
            } else {
                item.quantidadeUnidade += quantidade; // Atualiza a quantidade em unidades
            }
            produtoEncontrado = true;
            break;
        }
    }

    if (!produtoEncontrado) {
        Produto novoProduto = produtos[escolha - 1];
        novoProduto.quantidadeKg = formaCompra == 1 ? quantidade : 0; // Inicializa kg
        novoProduto.quantidadeUnidade = formaCompra == 2 ? quantidade : 0; // Inicializa unidades
        carrinho.push_back(novoProduto);
    }

    cout << "Adicionou ao carrinho " << quantidade << (formaCompra == 1 ? " kg" : " unidades") << " de " << produtos[escolha - 1].nome << "\n";
}

int continuarComprando() {
    cout << "=========================================================================================================\n";
    cout << "1 - Deseja adicionar mais produtos  \n";
    cout << "2 - Finalizar compra                \n";
    cout << "3 - Cancelar compra                 \n";
    cout << "=========================================================================================================\n";
    int opcao = obterOpcaoValida(1, 3);
}

// Função para selecionar o método de pagamento
void selecionarMetodoPagamento(float total, float& desconto, float& totalComDesconto, string& tipoPagamento) {
    limparTela(); 
    cout << "=========================================================================================================\n";
    mudaCor(1,6); 
    cout << "                                          TIPO DE PAGAMENTO                                              \n";
    mudaCor(1); 
    cout << "=========================================================================================================\n";
    cout << "1. Dinheiro (10% de desconto)\n";
    cout << "2. Pix (5% de desconto)\n";
    cout << "3. Cartao de Credito (sem desconto)\n";
    cout << "=========================================================================================================\n";
    cout << "Escolha uma opcao: ";
    int opcao = obterOpcaoValida(1, 3);
    calcularDesconto(total, opcao, desconto, totalComDesconto, tipoPagamento);
}

void finalizarCompra(const vector<Produto>& carrinho, float total) {
    if (carrinho.empty()) {
        cout << "Nenhum item adicionado ao carrinho.\n";
        return;
    }

    float desconto;
    float totalComDesconto;
    string tipoPagamento;

    // Seleciona o método de pagamento e calcula total com desconto
    selecionarMetodoPagamento(total, desconto, totalComDesconto, tipoPagamento);

    limparTela();

    // Obtém data e hora
    char dataHora[20]; // Declara um buffer para armazenar a data e hora
    obterDataHora(dataHora, sizeof(dataHora)); 
    mudaCor(6);
    cout << "*********************************************************************************************************\n";
    cout << "                                         NOTA FISCAL                                                     \n";
    cout << "*********************************************************************************************************\n";
    cout << "Data e Hora: " << dataHora << "\n";
    cout << "Tipo de Pagamento: " << tipoPagamento << "\n"; 
    cout << "*********************************************************************************************************\n";
    for (const auto& item : carrinho) {
        if (item.quantidadeKg > 0) {
            cout << item.nome << ": " << item.quantidadeKg << "kg - R$" << fixed << setprecision(2) << item.precoPorKg * item.quantidadeKg << "\n";
        }
        if (item.quantidadeUnidade > 0) {
            cout << item.nome << ": " << item.quantidadeUnidade << "un - R$" << fixed << setprecision(2) << item.precoPorUnidade * item.quantidadeUnidade << "\n";
        }
    }
    cout << "*********************************************************************************************************\n";
    cout << "Total: R$ " << total << "\n";
    cout << "Desconto: R$ " << desconto << "\n"; 
    cout << "Total com desconto: R$ " << totalComDesconto << "\n"; 
    cout << "*********************************************************************************************************\n";
    cout << "                             AGRADECEMOS SUA COMPRA VOLTE SEMPRE!!                                       \n";
    cout << "*********************************************************************************************************\n";
    salvarEstoque();

    mudaCor(15);
    cout << "Pressione qualquer tecla para voltar ao menu principal...\n"; 
    cin.ignore(); 
    cin.get(); 

    limparTela();
}

void realizarCompra(SOCKET servidorSocket, vector<Produto>& produtos) {
    limparTela(); 
    vector<Produto> carrinho; // Usando vector para o carrinho
    float total = 0.0;
    vector<Produto> estoqueOriginal = produtos; // Armazena o estoque original

    while (true) { // Loop para adicionar produtos
        int escolha = selecionaProduto();
        if (escolha == 0) {
            cancelarCompra(estoqueOriginal);
            return;
        }

        int formaCompra = opcaoCompra();
        float quantidade = obterQuantidade(formaCompra, escolha);
        if (quantidade <= 0) continue; // Se quantidade inválida, continua

        atualizarCarrinho(carrinho, formaCompra, quantidade, escolha, total);
        
        int opcaoContinuar = continuarComprando(); // Captura a opção escolhida
        if (opcaoContinuar == 2) {
            finalizarCompra(carrinho, total);
            return;
        } else if (opcaoContinuar == 3) {
            cancelarCompra(estoqueOriginal); // Cancela a compra
            return;
        }
    }
}

int obterIndiceValido(int tamanho){
     string verificaOpcao;
    int indice;
    while (true) { 
        cout << "Informe o indice do produto (1 a " << tamanho << "): ";
        cin >> verificaOpcao; 

        if (isValidInteger(verificaOpcao)) { 
            indice = stoi(verificaOpcao); 
            
           
            if (indice >= 1 && indice <= tamanho) {
                return indice - 1; 
            }
        }
        cout << "Indice invalido: \n";
    }
}

bool analisaFloatValido(const string& entrada, float& saida){
    if (entrada.empty()) return false;
    string entradaLimpa = entrada; 
    bool pontoDecimalEncontrado = false;

    for (char& c : entradaLimpa) {
        if (isdigit(c)) {
            continue; 
        }
        if (c == '.' || c == ',') {
            if (!pontoDecimalEncontrado) {
                pontoDecimalEncontrado = true;
                if (c == ',') {
                    c = '.'; 
                }
            } else {
                return false; 
            }
        } else {
            return false; 
        }
    }
    
    try {
        saida = stof(entradaLimpa);
    } catch (invalid_argument&) {
        return false; 
    } catch (out_of_range&) {
        return false; 
    }
    return true;
}

void validarProduto(Produto& novoProduto){
    string entrada;
    do {
        cout << "Informe o nome do produto: ";
        cin.ignore(); // Limpa o buffer
        getline(cin, novoProduto.nome);
    } while (!stringValida(novoProduto.nome));
    
    // Validação e conversão do preço por kg
    do {
        cout << "Informe o preço por kg: ";
        cin >> entrada;
    } while (!analisaFloatValido(entrada, novoProduto.precoPorKg));

    // Validação e conversão do preço por unidade
    do {
        cout << "Informe o preço por unidade: ";
        cin >> entrada;
    } while (!analisaFloatValido(entrada, novoProduto.precoPorUnidade));

    // Validação e conversão da quantidade em kg
    do {
        cout << "Informe a quantidade em kg: ";
        cin >> entrada;
    } while (!analisaFloatValido(entrada, novoProduto.quantidadeKg));

    // Validação da quantidade em unidades
    do {
        cout << "Informe a quantidade em unidades: ";
        cin >> entrada;
    } while (!isValidInteger(entrada));
    novoProduto.quantidadeUnidade = stoi(entrada);
}

void aguardarEntrada(){
    cout << "Pressione qualquer tecla para voltar ao menu...\n";
    cin.ignore(); 
    cin.get();    
    limparTela();
}

void estoqueAtual(){
    limparTela();
    mudaCor(1);
    cout <<"=========================================================================================================\n";
    mudaCor(1,6);
    cout << "                                      ESTOQUE ATUAL                                                     \n";
    mudaCor(1);
    cout <<"=========================================================================================================\n";
    for (size_t i = 0; i < produtos.size(); ++i) {
        const auto& produto = produtos[i];
        cout << (i+1)<< ". " << produto.nome << " - R$ " << produto.precoPorKg << " por kg - "
             << "R$ " << produto.precoPorUnidade << " por unidade - "
             << "Quantidade: " << produto.quantidadeKg << " kg - "
             << "Quantidade Unidades: " << produto.quantidadeUnidade << endl;
    cout <<"=========================================================================================================\n";
    }
}

void verificarSenha(){
    limparTela();
    const string senhaCorreta = "admin123"; 
    string senhaInformada;
    mudaCor(1);
    cout <<"=========================================================================================================\n";
    mudaCor(1,6);
    cout <<"                                       AREA ADMINISTRATIVA                                               \n";
    cout <<"                               ACESSO SOMENTE PARA PESSOAS AUTORIZADAS                                   \n";
    mudaCor(1);
    cout <<"=========================================================================================================\n";
    while (true) {
        cout << "Digite a senha: ";
        cin >> senhaInformada;
        if (senhaInformada == senhaCorreta) {
            cout << "Senha valida\n";
            break; 
        } else {
            cout << "Senha invalida: \n";
        }
    }
}

void menuAdministrativo(){
    verificarSenha();
    limparTela();
    while (true) {
        mudaCor(1);
        cout <<"=========================================================================================================\n";
        mudaCor(1,6);
        cout << "                                      MENU ADMINISTRATIVO                                               \n";
        mudaCor(1);
        cout <<"=========================================================================================================\n";
        cout << "1. Adicionar Produto\n";
        cout << "2. Remover Produto\n";
        cout << "3. Alterar Produto\n";
        cout << "4. Sair da area administrativa\n"; 
        cout <<"=========================================================================================================\n";
        cout << "Escolha uma opcao: ";
        int opcao = obterOpcaoValida(1, 4);

        switch (opcao) {
            case 1:
                //adicionarProduto();
                break;
            case 2:
                //removerProduto();
                break;
            case 3:
                //alterarProduto();
                break;
            case 4:
                cout << "Saindo da área administrativa...\n";
                return; 
            default:
                cout << "Opcao invalida: \n";
                break;
        }
    }
}



// Função que exibe o menu principal
void menuPrincipal(SOCKET clienteSocket) {
    while (true) {
        limparTela(); 
        mudaCor(1);
        cout << "=========================================================================================================\n";
        mudaCor(0,6); 
        cout << "                                            CAIXA HORTIFRUTI                                             \n";
        mudaCor(1); 
        cout << "=========================================================================================================\n";
        cout << "1. Fazer uma compra \n";
        cout << "2. Area Administrativa \n"; 
        cout << "3. Encerrar o caixa  \n"; 
        cout << "=========================================================================================================\n";
        cout << "Escolha uma opcao: ";
        int opcao = obterOpcaoValida(1, 3);

        switch (opcao) {
            case 1:
                realizarCompra(clienteSocket, produtos);
                break;
            case 2:
                menuAdministrativo();
                break;
            case 3:
                cout << "Sistema caixa encerrado...\n";
                closesocket(clienteSocket); 
                WSACleanup(); 
                return; 
        }
    }
}

int main() {

    struct sockaddr_in servidorAddr; // Estrutura para armazenar informações do servidor
    SOCKET clienteSocket = inicializarSocket(); // Inicializa o socket do cliente

    conectarServidor(clienteSocket, servidorAddr); 
    receberProdutos(clienteSocket); 
    menuPrincipal(clienteSocket); 
    
    return 0; 
}