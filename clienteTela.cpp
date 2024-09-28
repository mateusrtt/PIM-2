#include <iostream>
#include <vector>
#include <locale.h>
using namespace std;

int opcVenda, opcSelecao, opcFinalizarAdicionar, opcPagamento;
float qtdProduto, pesoProduto, vlrUnitario, vlrMultiplicado, valorTotal, valorAserPago;
string nomeProduto, formaPagamento, valorUnitarioStr;

vector<string> nomesProdutos;   // Vetor para armazenar nomes dos produtos
    vector<float> quantidades;        // Vetor para armazenar quantidades
    vector<double> valoresTotais;   // Vetor para armazenar valores totais

void menuPrincipal () {
    cout << "====================================" << endl;
    cout << "||        CAIXA HORTIFRUTI        ||" << endl;
    cout << "====================================" << endl;
    cout << "|| 1 - Ler produto                ||" << endl;
    cout << "|| 2 - Cancelar compra            ||" << endl;
    cout << "====================================" << endl;
    cout << "Escolha uma opção: ";
    cin >> opcVenda;
}

void menuSelecao () {
    cout << "====================================" << endl;
    cout << "||         MENU DE SELEÇÃO        ||" << endl;
    cout << "====================================" << endl;
    cout << "|| Escolha o tipo do produto      ||" << endl;
    cout << "|| 1 - Por quantidade             ||" << endl;
    cout << "|| 2 - Por peso                   ||" << endl;
    cout << "====================================" << endl;
    cout << "Escolha a opção: ";
    cin >> opcSelecao;
}

void vetorProdutos() {
    // Exibe os produtos adicionados, com suas quantidades e valores totais
    //cout << "\nProdutos cadastrados:" << endl;
    for (int i = 0; i < nomesProdutos.size(); i++) {
        cout << i + 1 << ". " << nomesProdutos[i] 
             << " - Quantidade: " << quantidades[i] 
             << ", Valor total: R$ " << valoresTotais[i] << endl;
    }
}

void finalizarAdicionar () {
    cout << "====================================" << endl;
    cout << "|| 1 - finaliza a compra          ||" << endl;
    cout << "|| 2 - Adicionar mais um produto  ||" << endl;
    cout << "====================================" << endl;
    cout << "Escolha a opção: ";
    cin >> opcFinalizarAdicionar;
}

void menuPagamento() {
    cout << "====================================" << endl;
    cout << "||         MENU DE PAGAMENTO       ||" << endl;
    cout << "====================================" << endl;
    cout << "|| 1 - A vista (10% de desconto)   ||" << endl;
    cout << "|| 2 - Pix (5% de desconto)        ||" << endl;
    cout << "|| 3 - Cartão (3% de juros)        ||" << endl;
    cout << "====================================" << endl;
    cout << "Escolha a opção: ";
    cin >> opcPagamento;
}

void menuNF() {
    cout << "====================================" << endl;
    cout << "||         NOTA FISCAL            ||" << endl;
    cout << "====================================" << endl;
    cout << "|| Forma de pagamento " << formaPagamento <<"     ||" << endl;
    cout << "====================================" << endl;
    vetorProdutos();
    cout << "Total " << valorTotal << endl;
    cout << "Total com desconto/juros: " << valorAserPago << endl;
    cout << "====================================" << endl;
}

// Função para substituir vírgula por ponto na string
float converterValorUnitario(string valorStr) {
    for (char &ch : valorStr) {
        if (ch == ',') {
            ch = '.';  // Substitui a vírgula pelo ponto
        }
    }
    return stof(valorStr);  // Converte string para float
}

int main() {
  setlocale(LC_ALL, "pt_BR_utf8");
  menuPrincipal();
    if (opcVenda == 2 || opcVenda != 1) {
        cout << "Compra cancelada..." << endl;
    }

    
    while (true){
      

       if (opcVenda == 1) {
        system("cls");
        menuSelecao();

        cin.ignore();
        cout << "Digite o nome do produto: ";
        
        getline(cin, nomeProduto);
        cout << "Digite a quantidade: ";
        cin >> qtdProduto;
        cout << "Digite o valor unitario (use vírgula ou ponto): ";
        cin.ignore();
        getline(cin, valorUnitarioStr);

        // Converter valor com vírgula para ponto
            vlrUnitario = converterValorUnitario(valorUnitarioStr);
            vlrMultiplicado = qtdProduto * vlrUnitario;

        // Armazena as informações nos vetores correspondentes
        nomesProdutos.push_back(nomeProduto);
        quantidades.push_back(qtdProduto);
        valoresTotais.push_back(vlrMultiplicado);

        cout << nomeProduto << " foi adicionado ao carinho..." << endl;
        cout << "Unidade: " << qtdProduto << " total: " << vlrMultiplicado << endl;
        valorTotal += vlrMultiplicado;
        }else if (opcVenda == 2) {
            system("cls");
            menuSelecao();

            cin.ignore();
            cout << "Digite o nome do produto: ";
            getline(cin, nomeProduto);

            cout << "Digite o peso: ";
            cin >> pesoProduto;
            cout << "Digite o valor unitario: ";
            cin >> vlrUnitario;
            vlrMultiplicado = qtdProduto * vlrUnitario;

            // Armazena as informações nos vetores correspondentes
            nomesProdutos.push_back(nomeProduto);
            quantidades.push_back(qtdProduto);
            valoresTotais.push_back(vlrMultiplicado);

            cout << nomeProduto << " foi adicionado ao carinho..." << endl;
            cout << "Peso: " << qtdProduto << " total: " << vlrMultiplicado << endl;
            valorTotal += vlrMultiplicado;
            }
            
          finalizarAdicionar();
          if (opcFinalizarAdicionar == 1) {
            
            break;   
        }
    }
    system("cls");
    menuPagamento();
    if (opcPagamento == 1) {
       valorAserPago = valorTotal * 0.90;
       cout << "Valor a ser pago a vista: " << valorAserPago << endl;
       formaPagamento = "a vista"; 
    }else if ( opcPagamento == 2) {
        valorAserPago = valorTotal * 0.95;
        cout << "Valor a ser pago por pix: " << valorAserPago << endl;
        formaPagamento = "por pix";
    }else if (opcPagamento == 3) {
        valorAserPago = valorTotal * 1.03;
        cout << "Valor a ser pago por cartao: " << valorAserPago << endl;
        formaPagamento = "por cartão";
    }   
    menuNF();
return 0;

}