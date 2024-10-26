#include "cliente.h"
#include "sockets.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <algorithm>
using namespace std;


/**
 * @brief Vetor global que armazena produtos recebidos.
 */
vector<Produto> produtos; 

/**
 * @brief Vetor global que armazena o estoque original de produtos.
 */
vector<Produto> estoqueOriginal; 


/**
 * @brief Recebe produtos de um servidor e os armazena em vetores.
 *
 * Esta função recebe uma lista de produtos via socket e armazena
 * as informações nos vetores fornecidos.
 *
 * @param clienteSocket Socket do cliente conectado ao servidor.
 * @param produtos Vetor onde os produtos recebidos serão armazenados.
 * @param estoqueOriginal Vetor onde o estoque original será armazenado.
 */
void receberProdutos(SOCKET clienteSocket, vector<Produto>& produtos, vector<Produto>& estoqueOriginal) {
    int numeroDeProdutos;
    if (recv(clienteSocket, (char*)&numeroDeProdutos, sizeof(int), 0) == SOCKET_ERROR) {
        cout << "Erro ao receber dados do servidor. Codigo de erro: " << WSAGetLastError() << "\n";
        closesocket(clienteSocket);
        WSACleanup();
        exit(1);
    }

    for (int i = 0; i < numeroDeProdutos; i++) {
        Produto produto;
        int nomeTamanho;
        if (recv(clienteSocket, (char*)&nomeTamanho, sizeof(nomeTamanho), 0) == SOCKET_ERROR) {
            cout << "Erro ao receber o tamanho do nome do produto. Código de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }

        vector<char> nomeBuffer(nomeTamanho + 1);
        if (recv(clienteSocket, nomeBuffer.data(), nomeTamanho, 0) == SOCKET_ERROR) {
            cout << "Erro ao receber o nome do produto. Código de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }

        nomeBuffer[nomeTamanho] = '\0'; 
        produto.nome = string(nomeBuffer.data());

        
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
        estoqueOriginal.push_back(produto); 
    }
}

/**
 * @brief Obtém a data e hora atuais formatadas.
 *
 * A função formata a data e hora no formato "dd/mm/aaaa hh:mm:ss" e
 * armazena no buffer fornecido.
 *
 * @param buffer Ponteiro para o array onde a data e hora serão armazenadas.
 * @param tamanho Tamanho máximo do buffer.
 */
void obterDataHora(char *buffer, size_t tamanho) {
    time_t t; 
    struct tm *tm_info; 

    time(&t); 
    tm_info = localtime(&t); 
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M:%S", tm_info); 
}

/**
 * @brief Valida se uma string representa um inteiro.
 *
 * @param entrada A string a ser validada.
 * @return true Se a string é um número inteiro válido.
 * @return false Se a string não é um número inteiro válido.
 */
bool inteiroValido(const string& entrada) {
    if (entrada.empty()) return false; 
    return all_of(entrada.begin(), entrada.end(), ::isdigit); 
}

/**
 * @brief Valida se uma string contém apenas letras e espaços.
 *
 * @param entrada A string a ser validada.
 * @return true se a string é válida.
 * @return false se a string é inválida.
 */
bool stringValida(const string& entrada) {
    return !entrada.empty() && all_of(entrada.begin(), entrada.end(), [](unsigned char c) {
        return isalpha(c) || isspace(c);
    });
}

/**
 * @brief Valida se uma string representa um número decimal.
 *
 * @param entrada A string a ser validada.
 * @param saida Variável onde o valor decimal é armazenado, se válido.
 * @return true Se a string é um número decimal válido.
 * @return false Se a string não é um número decimal válido.
 */
bool floatValido(const string& entrada, float& saida) {
    if (entrada.empty()) return false;

    string entradaLimpa = entrada;
    for (char& c : entradaLimpa) {
        if (c == ',') {
            c = '.';
        }
    }
    
    try {
        saida = stof(entradaLimpa);
    } catch (const invalid_argument&) {
        return false; 
    } catch (const out_of_range&) {
        return false; 
    }

    int pontoDecimalCount = 0;
    for (char c : entradaLimpa) {
        if (!isdigit(c) && c != '.') {
            return false; 
        }
        if (c == '.') {
            pontoDecimalCount++;
            if (pontoDecimalCount > 1) return false; 
        }
    }
    return true;
}

/**
 * @brief Altera a cor do texto e do fundo no console.
 *
 * @param corTexto Código da cor do texto.
 * @param corFundo Código da cor de fundo (opcional, padrão é 0).
 */
void mudaCor(int corTexto, int corFundo) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, corTexto | (corFundo << 4));
}

/**
 * @brief Limpa a tela do console.
 */
void limparTela() {
    system("cls"); 
}

/**
 * @brief Salva o estoque de produtos em um arquivo.
 *
 * Os dados dos produtos são escritos no arquivo "estoque.txt".
 */
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

/**
 * @brief Obtém uma opção válida dentro de um intervalo.
 *
 * @param min Valor mínimo da opção.
 * @param max Valor máximo da opção.
 * @return int é a opção escolhida pelo usuário.
 */
int obterOpcaoValida(int min, int max) {
    string verificaOpcao;
    int opcao;

    while (true) {  
        cin >> verificaOpcao; 
        if (inteiroValido(verificaOpcao)) { 
            opcao = stoi(verificaOpcao);
            if (opcao >= min && opcao <= max) {
                return opcao; 
            }
        }
        cout << "Opçao invalida: "; 
    }
}

/**
 * @brief Calcula o desconto baseado na forma de pagamento.
 *
 * @param total O valor total da compra.
 * @param opcao A opção de pagamento escolhida.
 * @param desconto Referência para armazenar o valor do desconto calculado.
 * @param totalComDesconto Referência para armazenar o total com desconto.
 * @param tipoPagamento Referência para armazenar o tipo de pagamento.
 */
void calcularDesconto(float total, int opcao, float& desconto, float& totalComDesconto, string& tipoPagamento) {
    desconto = 0.0; 
    totalComDesconto = total; 
    
    if (opcao == 1) {
        desconto = total * 0.10; 
        tipoPagamento = "Dinheiro";
    } else if (opcao == 2) {
        desconto = total * 0.05; 
        tipoPagamento = "Pix";
    } else {
        tipoPagamento = "Cartao de Credito"; 
    }
    totalComDesconto -= desconto; 
}

/**
 * @brief Exibe o menu de produtos.
 */
void exibirMenu() {
    limparTela();
    mudaCor(1); 
    cout << "=========================================================================================================\n";
    mudaCor(1,6);
    cout << "                                              PRODUTOS                                                   \n";
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
}

/**
 * @brief Seleciona um produto do menu.
 *
 * @return int é o número do produto selecionado pelo usuário.
 */
int selecionaProduto(){
    exibirMenu();
    cout << "Digite o numero do produto: ";
    int opcao = obterOpcaoValida(1, produtos.size()); 
}

/**
 * @brief Cancela a compra e restaura o estoque original.
 *
 * @param estoqueOriginal Vetor que armazena o estoque original.
 */
void cancelarCompra(vector<Produto>& estoqueOriginal){
    produtos = estoqueOriginal; 
    salvarEstoque(); 
}

/**
 * @brief Obtém a opção de compra (por kg ou unidade).
 *
 * @return int é a opção escolhida pelo usuário.
 */
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

/**
 * @brief Valida a quantidade de produtos solicitados.
 *
 * @param formaCompra Indica se a compra é por kg ou unidade.
 * @param verificaQuantidade String com a quantidade fornecida pelo usuário.
 * @param quantidade Referência para armazenar a quantidade válida.
 * @param escolha Índice do produto selecionado.
 * @return true se a quantidade é válida.
 * @return false se a quantidade é inválida.
 */
bool validarQuantidade(int formaCompra, const string& verificaQuantidade, float& quantidade, int escolha) {
    if (formaCompra == 1) { 
        if (floatValido(verificaQuantidade, quantidade)) {
            quantidade = stof(verificaQuantidade); 
            if (quantidade > 0 && quantidade <= produtos[escolha - 1].quantidadeKg) {
                produtos[escolha - 1].quantidadeKg -= quantidade; 
                return true;
            }
        }
    } else { 
        if (inteiroValido(verificaQuantidade)) {
            quantidade = stoi(verificaQuantidade); 
            if (quantidade > 0 && quantidade <= produtos[escolha - 1].quantidadeUnidade) {
                produtos[escolha - 1].quantidadeUnidade -= quantidade; 
                return true;
            }
        }
    }
    return false; 
}

/**
 * @brief Exibe mensagem de estoque insuficiente e retorna uma opção.
 *
 * @return int é a opção escolhida pelo usuário.
 */
int estoqueInsuficiente(){
    cout << "Estoque insuficiente\n";
    cout << " 1 - Deseja tentar novamente\n";
    cout << " 2 - Voltar ao menu de produtos?\n";
    cout << "=========================================================================================================\n";
    cout << "Esoclha uma opcao: ";
    return obterOpcaoValida(1, 2);
}

/**
 * @brief Obtém a quantidade de produtos solicitados.
 *
 * @param formaCompra Indica se a compra é por kg ou unidade.
 * @param escolha Índice do produto selecionado.
 * @return float a quantidade solicitada pelo usuário.
 */
float obterQuantidade(int formaCompra, int escolha) {
    string verificaQuantidade; 
    float quantidade = 0.0;

    while (true) {
        if (formaCompra == 1) {
            cout << "Informe o peso do produto (em kg): ";
        } else {
            cout << "Informe a quantidade do produto (em unidades): ";
        }
        cin >> verificaQuantidade;

        replace(verificaQuantidade.begin(), verificaQuantidade.end(), ',', '.');

        if (validarQuantidade(formaCompra, verificaQuantidade, quantidade, escolha)) {
            return quantidade;
        }

        int opcao = estoqueInsuficiente();

        if (opcao == 2) {
            return -1; 
        }
    }
}

/**
 * @brief Atualiza o carrinho de compras com o produto selecionado.
 *
 * @param carrinho Vetor onde os produtos comprados são armazenados.
 * @param formaCompra Indica se a compra é por kg ou unidade.
 * @param quantidade A quantidade do produto a ser adicionada.
 * @param escolha Índice do produto selecionado.
 * @param total Referência para armazenar o total acumulado da compra.
 */
void atualizarCarrinho(vector<Produto>& carrinho, int formaCompra, float quantidade, int escolha, float& total) {
    if (formaCompra == 1) { 
        total += produtos[escolha - 1].precoPorKg * quantidade;
    } else { 
        total += produtos[escolha - 1].precoPorUnidade * quantidade;
    }

    salvarEstoque();

    bool produtoEncontrado = false;
    for (auto& item : carrinho) {
        if (item.nome == produtos[escolha - 1].nome) {
            if (formaCompra == 1) {
                item.quantidadeKg += quantidade; 
            } else {
                item.quantidadeUnidade += quantidade; 
            }
            produtoEncontrado = true;
            break;
        }
    }

    if (!produtoEncontrado) {
        Produto novoProduto = produtos[escolha - 1];
        novoProduto.quantidadeKg = formaCompra == 1 ? quantidade : 0; 
        novoProduto.quantidadeUnidade = formaCompra == 2 ? quantidade : 0; 
        carrinho.push_back(novoProduto);
    }

    cout << "Adicionou ao carrinho " << quantidade << (formaCompra == 1 ? " kg" : " unidades") << " de " << produtos[escolha - 1].nome << "\n";
}

/**
 * @brief Pergunta ao usuário se deseja continuar comprando.
 *
 * @return int é a opção escolhida pelo usuário.
 */
int continuarComprando() {
    cout << "=========================================================================================================\n";
    cout << "1 - Deseja adicionar mais produtos  \n";
    cout << "2 - Finalizar compra                \n";
    cout << "3 - Cancelar compra                 \n";
    cout << "=========================================================================================================\n";
    int opcao = obterOpcaoValida(1, 3);
}

/**
 * @brief Seleciona o método de pagamento e calcula descontos.
 *
 * @param total O valor total da compra.
 * @param desconto Referência para armazenar o desconto calculado.
 * @param totalComDesconto Referência para armazenar o total com desconto.
 * @param tipoPagamento Referência para armazenar o tipo de pagamento.
 */
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

/**
 * @brief Aguarda a entrada do usuário antes de voltar ao menu.
 */
void aguardarEntrada(){
    cout << "Pressione qualquer tecla para voltar ao menu...\n";
    cin.ignore(); 
    cin.get();    
    limparTela();
}

/**
 * @brief Finaliza a compra e exibe a nota fiscal.
 *
 * @param carrinho Vetor que contém os produtos comprados.
 * @param total O valor total da compra.
 */
void finalizarCompra(const vector<Produto>& carrinho, float total) {
    if (carrinho.empty()) {
        cout << "Nenhum item adicionado ao carrinho.\n";
        return;
    }

    float desconto;
    float totalComDesconto;
    string tipoPagamento;

    selecionarMetodoPagamento(total, desconto, totalComDesconto, tipoPagamento);

    limparTela();

    char dataHora[20]; 
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
    aguardarEntrada();
}

/**
 * @brief Realiza o processo de compra de produtos.
 *
 * @param servidorSocket Socket do servidor para comunicação.
 * @param produtos Vetor com os produtos disponíveis para compra.
 */
void realizarCompra(SOCKET servidorSocket, vector<Produto>& produtos) {
    limparTela(); 
    vector<Produto> carrinho; 
    float total = 0.0;
    vector<Produto> estoqueOriginal = produtos; 

    while (true) { 
        int escolha = selecionaProduto();
        if (escolha == 0) {
            cancelarCompra(estoqueOriginal);
            return;
        }

        int formaCompra = opcaoCompra();
        float quantidade = obterQuantidade(formaCompra, escolha);
        if (quantidade == -1) {
            continue; 
        }

        atualizarCarrinho(carrinho, formaCompra, quantidade, escolha, total);
        
        int opcaoContinuar = continuarComprando(); 
        if (opcaoContinuar == 2) {
            finalizarCompra(carrinho, total);
            return;
        } else if (opcaoContinuar == 3) {
            cancelarCompra(estoqueOriginal); 
            return;
        }
    }
}

/**
 * @brief Obtém um índice válido do produto a partir da entrada do usuário.
 * 
 * Essa função solicita ao usuário que informe um índice para um produto
 * e verifica se o índice está dentro do intervalo válido (1 a tamanho).
 * 
 * @param tamanho O número total de produtos disponíveis.
 * @return int O índice válido do produto (base 0).
 */
int obterIndiceValido(int tamanho){
    string verificaOpcao;
    int indice;
    while (true) { 
        cout << "Informe o indice do produto (1 a " << tamanho << "): ";
        cin >> verificaOpcao; 

        if (inteiroValido(verificaOpcao)) { 
            indice = stoi(verificaOpcao); 
            
           
            if (indice >= 1 && indice <= tamanho) {
                return indice - 1; 
            }
        }
        cout << "Indice invalido: \n";
    }
}

/**
 * @brief Valida as informações de um novo produto.
 * 
 * Solicita ao usuário informações sobre o novo produto e valida cada entrada,
 * incluindo o nome, preço por kg, preço por unidade, quantidade em kg e quantidade em unidades.
 * 
 * @param novoProduto Referência ao produto a ser validado.
 */
void validarProduto(Produto& novoProduto) {
    string entrada;
    // Validação do nome do produto
    do {
        cout << "Informe o nome do produto: ";
        cin.ignore(); 
        getline(cin, novoProduto.nome);
        if (!stringValida(novoProduto.nome)) {
            cout << "Nome do produto invalido: " << endl;
        }
    } while (!stringValida(novoProduto.nome));

    // Validação e conversão do preço por kg
    do {
        cout << "Informe o preco por kg: ";
        cin >> entrada;
        if (!floatValido(entrada, novoProduto.precoPorKg)) {
            cout << "Preco por kg invalido: " << endl;
        }
    } while (!floatValido(entrada, novoProduto.precoPorKg));

    // Validação e conversão do preço por unidade
    do {
        cout << "Informe o preco por unidade: ";
        cin >> entrada;
        if (!floatValido(entrada, novoProduto.precoPorUnidade)) {
            cout << "Preco por unidade invalido: " << endl;
        }
    } while (!floatValido(entrada, novoProduto.precoPorUnidade));

    // Validação e conversão da quantidade em kg
    do {
        cout << "Informe a quantidade em kg: ";
        cin >> entrada;
        if (!floatValido(entrada, novoProduto.quantidadeKg)) {
            cout << "Quantidade em kg invalida: " << endl;
        }
    } while (!floatValido(entrada, novoProduto.quantidadeKg));

    // Validação da quantidade em unidades
    do {
        cout << "Informe a quantidade em unidades: ";
        cin >> entrada;
        if (!inteiroValido(entrada)) {
            cout << "Quantidade em unidades invalida: " << endl;
        }
    } while (!inteiroValido(entrada));
    
    novoProduto.quantidadeUnidade = stoi(entrada);
}

/**
 * @brief Adiciona um novo produto à lista de produtos.
 * 
 * Exibe o menu, valida as informações do novo produto e o adiciona à lista.
 */
void adicionarProduto(){
    exibirMenu();
    Produto novoProduto;
    validarProduto(novoProduto); 
    produtos.push_back(novoProduto); 
    salvarEstoque();
    exibirMenu();
    cout << "Produto adicionado com sucesso!\n";
    aguardarEntrada();
}

/**
 * @brief Remove um produto da lista.
 * 
 * Exibe o menu e solicita ao usuário um índice para remover o produto correspondente.
 */
void removerProduto(){
    exibirMenu();
    int indice = obterIndiceValido(produtos.size()); 
    produtos.erase(produtos.begin() + indice); 
    salvarEstoque();
    exibirMenu();
    cout << "Produto removido com sucesso!\n";
    aguardarEntrada();
}

/**
 * @brief Altera as informações de um produto existente.
 * 
 * Exibe o menu, solicita um índice e valida as novas informações do produto.
 */
void alterarProduto(){
    exibirMenu();
    int indice = obterIndiceValido(produtos.size()); 
    Produto& produto = produtos[indice]; 
    validarProduto(produto); 
    salvarEstoque(); 
    cout << "Produto alterado com sucesso!\n";
    aguardarEntrada();
}

/**
 * @brief Verifica a senha do administrador.
 * 
 * Solicita que o usuário insira a senha correta para acessar a área administrativa.
 */
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

/**
 * @brief Exibe o menu administrativo para gerenciamento de produtos.
 * 
 * Após a validação da senha, o menu permite adicionar, remover ou alterar produtos.
 */
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
                adicionarProduto();
                break;
            case 2:
                removerProduto();
                break;
            case 3:
                alterarProduto();
                break;
            case 4:
                cout << "Saindo da area administrativa...\n";
                return; 
            default:
                cout << "Opcao invalida: \n";
                break;
        }
    }
}

/**
 * @brief Exibe o menu principal do sistema.
 * 
 * Permite ao usuário realizar uma compra, acessar a área administrativa ou encerrar o caixa.
 * 
 * @param clienteSocket Socket do cliente para comunicação.
 */
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