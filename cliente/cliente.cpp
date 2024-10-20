#include "cliente.h"
#include "sockets.h"

vector<Produto> produtos; // Usando vector para gerenciar a lista de produtos
vector<Produto> estoqueOriginal; // Estoque original para restauração

// Função para receber produtos do servidor
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

        // Usando std::string para gerenciar a memória
        vector<char> nomeBuffer(nomeTamanho + 1);
        if (recv(clienteSocket, nomeBuffer.data(), nomeTamanho, 0) == SOCKET_ERROR) {
            cout << "Erro ao receber o nome do produto. Código de erro: " << WSAGetLastError() << "\n";
            closesocket(clienteSocket);
            WSACleanup();
            exit(1);
        }

        nomeBuffer[nomeTamanho] = '\0'; // Adiciona o terminador de string
        produto.nome = string(nomeBuffer.data());

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

bool inteiroValido(const string& entrada) {
    if (entrada.empty()) return false; // Verifica se a entrada não está vazia
    return all_of(entrada.begin(), entrada.end(), ::isdigit); // Verifica se todos os caracteres são dígitos
}

bool stringValida(const string& entrada) {
    return !entrada.empty() && all_of(entrada.begin(), entrada.end(), [](unsigned char c) {
        return isalpha(c) || isspace(c);
    });
}

bool floatValido(const string& entrada, float& saida) {
    if (entrada.empty()) return false;

    // Substitui vírgula por ponto
    string entradaLimpa = entrada;
    for (char& c : entradaLimpa) {
        if (c == ',') {
            c = '.';
        }
    }
    // Tenta converter a string para float
    try {
        saida = stof(entradaLimpa);
    } catch (const invalid_argument&) {
        return false; // Não é um número válido
    } catch (const out_of_range&) {
        return false; // O número está fora do intervalo
    }

    // Verifica se a string contém apenas dígitos e no máximo um ponto
    int pontoDecimalCount = 0;
    for (char c : entradaLimpa) {
        if (!isdigit(c) && c != '.') {
            return false; // Caractere inválido
        }
        if (c == '.') {
            pontoDecimalCount++;
            if (pontoDecimalCount > 1) return false; // Mais de um ponto decimal
        }
    }
    return true;
}

void mudaCor(int corTexto, int corFundo) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, corTexto | (corFundo << 4));
}

// Função para limpar a tela do console
void limparTela() {
    system("cls"); 
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
        if (inteiroValido(verificaOpcao)) { // Verifica se é um número inteiro
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

int selecionaProduto(){
    exibirMenu();
    cout << "Digite o numero do produto: ";
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
        if (floatValido(verificaQuantidade, quantidade)) {
            quantidade = stof(verificaQuantidade); // Converte para float
            if (quantidade > 0 && quantidade <= produtos[escolha - 1].quantidadeKg) {
                produtos[escolha - 1].quantidadeKg -= quantidade; // Atualiza a quantidade em kg
                return true;
            }
        }
    } else { // Compra em unidades
        if (inteiroValido(verificaQuantidade)) {
            quantidade = stoi(verificaQuantidade); // Converte para int
            if (quantidade > 0 && quantidade <= produtos[escolha - 1].quantidadeUnidade) {
                produtos[escolha - 1].quantidadeUnidade -= quantidade; // Atualiza a quantidade em unidades
                return true;
            }
        }
    }
    return false; // Se não passar nas validações
}

int estoqueInsuficiente(){
    cout << "Estoque insuficiente\n";
    cout << " 1 - Deseja tentar novamente\n";
    cout << " 2 - Voltar ao menu de produtos?\n";
    cout << "=========================================================================================================\n";
    cout << "Esoclha uma opcao: ";
    return obterOpcaoValida(1, 2);
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

        int opcao = estoqueInsuficiente();

        if (opcao == 2) {
            return -1; // Indica que o usuário deseja voltar
        }
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

void aguardarEntrada(){
    cout << "Pressione qualquer tecla para voltar ao menu...\n";
    cin.ignore(); 
    cin.get();    
    limparTela();
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
    aguardarEntrada();
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
        if (quantidade == -1) {
            continue; // Volta ao menu se a opção for voltar
        }

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

        if (inteiroValido(verificaOpcao)) { 
            indice = stoi(verificaOpcao); 
            
           
            if (indice >= 1 && indice <= tamanho) {
                return indice - 1; 
            }
        }
        cout << "Indice invalido: \n";
    }
}

void validarProduto(Produto& novoProduto) {
    string entrada;
    // Validação do nome do produto
    do {
        cout << "Informe o nome do produto: ";
        cin.ignore(); // Limpa o buffer
        getline(cin, novoProduto.nome);
        if (!stringValida(novoProduto.nome)) {
            cout << "Nome do produto invalido: " << endl;
        }
    } while (!stringValida(novoProduto.nome));

    // Validação e conversão do preço por kg
    do {
        cout << "Informe o preço por kg: ";
        cin >> entrada;
        if (!floatValido(entrada, novoProduto.precoPorKg)) {
            cout << "Preço por kg invalido: " << endl;
        }
    } while (!floatValido(entrada, novoProduto.precoPorKg));

    // Validação e conversão do preço por unidade
    do {
        cout << "Informe o preço por unidade: ";
        cin >> entrada;
        if (!floatValido(entrada, novoProduto.precoPorUnidade)) {
            cout << "Preço por unidade invalido: " << endl;
        }
    } while (!floatValido(entrada, novoProduto.precoPorUnidade));

    // Validação e conversão da quantidade em kg
    do {
        cout << "Informe a quantidade em kg: ";
        cin >> entrada;
        if (!floatValido(entrada, novoProduto.quantidadeKg)) {
            cout << "Quantidade em kg inválida: " << endl;
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

void adicionarProduto(){
    limparTela();
    exibirMenu();
    Produto novoProduto;
    validarProduto(novoProduto); 
    produtos.push_back(novoProduto); 
    salvarEstoque();
    limparTela();
    exibirMenu();
    cout << "Produto adicionado com sucesso!\n";
    aguardarEntrada();
}

void removerProduto(){
    limparTela();
    exibirMenu();
    int indice = obterIndiceValido(produtos.size()); 
    produtos.erase(produtos.begin() + indice); 
    salvarEstoque();
    limparTela();
    exibirMenu();
    cout << "Produto removido com sucesso!\n";
    aguardarEntrada();
}

void alterarProduto(){
    limparTela();
    exibirMenu();
    int indice = obterIndiceValido(produtos.size()); 
    Produto& produto = produtos[indice]; 
    validarProduto(produto); 
    salvarEstoque(); 
    cout << "Produto alterado com sucesso!\n";
    aguardarEntrada();
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
                adicionarProduto();
                break;
            case 2:
                removerProduto();
                break;
            case 3:
                alterarProduto();
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