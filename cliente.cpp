#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <limits>
#include <algorithm>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080
using namespace std;


bool isValidInteger(const string& input) {
    // Verifica se todos os caracteres da string são dígitos
    return all_of(input.begin(), input.end(), ::isdigit);
}

int conectarServidor(int sock){
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    return connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
}

int iniciarEConectarSocket() {
    // Inicia o Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        cerr << "Erro ao inicializar o Winsock" << endl;
        return -1;
    }

    // Criação do socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Erro ao criar socket" << endl;
        WSACleanup(); 
        return -1;
    }

    // Tentativa de conexão com o servidor
    if (conectarServidor(sock) < 0){
        cerr << "Erro ao conectar ao servidor" << endl;
        cerr << "Código de erro: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }
    return sock; 
}

int menuPrincipal() {
    system("cls");
    int opcao;
    cout << "========================\n";
    cout << "    CAIXA HORTIFRUTI    \n";
    cout << "========================\n";
    cout << "1 - Ler produto\n";
    cout << "2 - Cancelar compra\n";
    cout << "========================\n";
    cout << "Escolha uma opcao: ";

    string verificaOpcao; //Variável para armazenar a entrada do tipo de produto
    while (true) {  // Valida a entrada do tipo de produto
        cin >>verificaOpcao; //Lê a entrada do usuário
        if (isValidInteger(verificaOpcao)) { // Verifica se a entrada é um número inteiro
        opcao = stoi(verificaOpcao); // Converte a string para inteiro
        if (opcao == 1 || opcao == 2) break;// Aceita apenas opções 1 ou 2
        }
        cout << "Opcao invalida. Tente novamente: ";
    }
    return opcao;
}

void lerProduto(string &carrinho, double &total) {
    system("cls");  // Limpa a tela antes de ler o produto
    int opcao;
    cout << "========================\n";
    cout << "      MENU SELECAO      \n";
    cout << "========================\n";
    cout << "Escolha o tipo de produto\n";
    cout << "1 - Por quantidade\n";
    cout << "2 - Por peso\n";
    cout << "========================\n";
    cout << "Escolha a opcao: ";

    string verificaOpcao; //Variável para armazenar a entrada do tipo de produto
    while (true) {  // Valida a entrada do tipo de produto
        cin >> verificaOpcao; //Lê a entrada do usuário
        if (isValidInteger(verificaOpcao)) { // Verifica se a entrada é um número inteiro
            opcao = stoi(verificaOpcao); // Converte a string para inteiro
            if (opcao == 1 || opcao == 2) break;// Aceita apenas opções 1 ou 2
        }
        cout << "Opcao invalida. Tente novamente: ";
    }

    
    string nome; //Variável para armazenar o nome do produto
    double valor; //Variável para armazenar o valor do produto

    if (opcao == 1) { //Se a opção escolhida for 1 (por quantidade)
        //Lógica para produtos por quantidade
        int quantidade;  //Variável para armazenar a quantidade
        cout << "Digite o nome do produto: ";
        cin.ignore(); //Ignora o newline deixado pelo cin anterior
        getline(cin, nome); //Lê o nome do produto

        // Valida a entrada da quantidade
        while (true) {
        cout << "Digite a quantidade: ";
        string lerQuantidade; //Variável para armazenar a entrada
        cin >> lerQuantidade;  //Lê a entrada do usuário

        if (isValidInteger(lerQuantidade)) { //Verifica se a entrada é um número inteiro
            quantidade = stoi(lerQuantidade); //Converte a string para inteiro
            if (quantidade > 0) { //Verifica se a quantidade é maior que 0
                break; //Sai do loop se a quantidade for válida
            } else {
                cout << "Quantidade inválida. Deve ser maior que zero." << endl;
            }
        } else {
            cout << "Quantidade inválida. Tente novamente." << endl;
        }
    }
        cout << "Digite o valor unitário: "; //Solicita o valor unitário do produto
        string valorUnitario; // Variável para armazenar a entrada do valor
        cin >> valorUnitario; // Lê a entrada do usuário
        replace(valorUnitario.begin(), valorUnitario.end(), ',', '.'); //Substitui vírgula por ponto
        valor = stod(valorUnitario); //Converte a string para double
        total += quantidade * valor; //Atualiza o total com a nova compra
        ostringstream oss; //Cria um objeto ostringstream para formatar a saída
        oss << fixed << setprecision(2) << (quantidade * valor); //Formata o valor total

        //Adiciona item ao carrinho
        carrinho += nome + ": " + to_string(quantidade) + " Qtd - R$ " + oss.str() + "\n";

    } else { //Se a opção escolhida for 2 (por peso)
        // Lógica para produtos por peso
        double peso; //Variável para armazenar o peso
        cout << "Digite o nome do produto: ";
        cin.ignore(); //Ignora o newline deixado pelo cin anterior
        getline(cin, nome); // Lê o nome do produto

        cout << "Digite o peso: "; //Solicita o peso do produto
        string lerPeso; //Variável para armazenar a entrada do peso
        cin >> lerPeso; //Lê a entrada do usuário
        replace(lerPeso.begin(), lerPeso.end(), ',', '.');//Substitui vírgula por ponto
        peso = stod(lerPeso); //Converte a string para double

        cout << "Digite o valor por Kg: "; //Solicita o valor por kg do produto
        string valorKg; //Variável para armazenar a entrada do valor
        cin >> valorKg; //Lê a entrada do usuário
        replace(valorKg.begin(), valorKg.end(), ',', '.'); //Substitui vírgula por ponto
        valor = stod(valorKg); //Converte a string para double

        total += peso * valor; //Atualiza o total com a nova compra

        ostringstream oss_peso, oss_valor; //Cria objetos ostringstream para formatar a saída
        oss_peso << fixed << setprecision(2) << peso; //Formata o peso            
        oss_valor << fixed << setprecision(2) << (peso * valor); //Formata o valor total
        //Adiciona item ao carrinho
        carrinho += nome + ": " + oss_peso.str() + " Kg - R$ " + oss_valor.str() + "\n";
    }
}


int main(){
    int sock = iniciarEConectarSocket();
     if (sock < 0){
        return 1;
     }

    string carrinho; // Inicializa o carrinho
    double total = 0.0; // Inicializa o total
     while (true) {
        int opcao = menuPrincipal(); // Chama o menu principal
        if (opcao == 1) { // Se a opção for 1, lê o produto
            lerProduto(carrinho, total); // Chama a função para ler produto
        } else if (opcao == 2) { // Se a opção for 2, cancela a compra
            cout << "Compra cancelada." << endl;
            carrinho.clear(); // Limpa o carrinho
            total = 0.0; // Reseta o total
            cout << "Pressione qualquer tecla para uma nova compra..." << endl;
            cin.ignore(); // Limpa o buffer do cin
            cin.get(); // Espera o usuário pressionar Enter
        }

}

closesocket(sock);
WSACleanup();
return 0;
}