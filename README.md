# Projeto: Sistema de Caixa e Controle de Estoque 

Este projeto foi desenvolvido em grupo como trabalho acadêmico na faculdade. Ele implementa um sistema de caixa e controle de estoque, com uma arquitetura cliente-servidor e comunicação via sockets. O sistema permite a gestão de produtos, controle de estoque, realização de compras, geração de nota fiscal e gerenciamento do inventário.

## Funcionalidades Principais

- **Gestão de Produtos:** Cadastro, edição e remoção de produtos.
- **Controle de Estoque:** Quantidades de produtos em kg ou unidades.
- **Sistema de Caixa:** Realiza cálculos com base nas seleções do cliente, incluindo opções de pagamento e aplicação de descontos.
- **Geração de Nota Fiscal:** Exibe a nota fiscal ao usuário e pergunta se deseja continuar ou finalizar a compra.
- **Menu Administrativo:** Permite o gerenciamento de estoque e verificação de senha para acesso.

## Estrutura de Arquivos

### PIM-2/cliente
- `main_cliente.cpp`,`cliente.cpp`, `cliente.h`: Implementação e cabeçalho das funções de conexão e manipulação do caixa.
- `sockets.cpp`, `sockets.h`: Implementação e cabeçalho para a comunicação via sockets.

### PIM-2/produto
- `produto.h`: Estrutura do Produto, com atributos como nome, preço por unidade, preço por kg, quantidade em kg e em unidades.

### PIM-2/servidor
- `main_servidor.cpp`, `servidor.cpp`, `servidor.h`: Implementação e cabeçalho das funções do servidor.
- `sockets.cpp`, `sockets.h`: Implementação e cabeçalho para a comunicação via sockets.

## Pré-requisitos

- Compilador C++ (ex: GCC ou MinGW para Windows).
- Biblioteca de Sockets para comunicação cliente-servidor.

### Compilando o Servidor e Cliente

No Windows, para compilar o cliente e o servidor, use o seguinte comando no terminal (PowerShell ou CMD):

```bash

## Comando para compilar (compile primeiro o servidor)

g++ -o servidor servidor/main_servidor.cpp servidor/servidor.cpp servidor/sockets.cpp -lws2_32
g++ -o cliente cliente/main_cliente.cpp cliente/cliente.cpp cliente/sockets.cpp -lws2_32

## Executando o Projeto

.\servidor.exe
.\cliente.exe               



