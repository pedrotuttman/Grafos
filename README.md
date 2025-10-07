# 🧮 Trabalho de Grafos – Parte 1  
### COS242 – Teoria dos Grafos | UFRJ  

![C++](https://img.shields.io/badge/language-C++17-blue.svg)  
![Status](https://img.shields.io/badge/status-Em%20desenvolvimento-success.svg)  
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)  
![UFRJ](https://img.shields.io/badge/UFRJ-Engenharia%20de%20Computação%20e%20Informação-yellow.svg)  

---

## 🎯 Objetivo  

Este projeto implementa uma **biblioteca para manipulação de grafos não-direcionados**, como parte da disciplina **COS242 – Teoria dos Grafos (UFRJ)**. O trabalho tem como meta projetar uma estrutura modular, eficiente e reutilizável para representar grafos e executar algoritmos clássicos de análise.  

A biblioteca oferece as seguintes funcionalidades:  
- Leitura de grafos a partir de **arquivos texto**;  
- Representação por **lista de adjacência** e **matriz de adjacência**;  
- Implementação de algoritmos de:  
  - **Busca em Largura (BFS)**  
  - **Busca em Profundidade (DFS)**  
  - **Cálculo de distâncias entre vértices**  
  - **Determinação do diâmetro (exato e aproximado)**  
  - **Identificação de componentes conexas**  
- Geração de **estatísticas e benchmarks** para análise de desempenho e uso de memória.  

---

## 🧩 Estrutura do Projeto  

```
.
├── main.cpp                     # Programa principal (interpreta os argumentos de execução)
├── graph.hpp                    # Implementação da classe Graph e métodos auxiliares
├── Estudo_de_Casos_Tabelas.xlsx # Resultados experimentais (tempo, memória, componentes, etc.)
├── trabalho-P1.pdf              # Enunciado oficial do trabalho (Parte 1)
└── README.md                    # Este arquivo
```

---

## ⚙️ Funcionalidades Implementadas  

A execução do programa é feita por linha de comando, com **flags configuráveis** que determinam o tipo de operação.  

### 🔧 Representações Suportadas  
- `--rep=list` → usa **lista de adjacência**  
- `--rep=matrix` → usa **matriz de adjacência**  

### 🧠 Principais Comandos  

| Comando | Descrição |
|----------|------------|
| `--in=arquivo.txt` | Arquivo de entrada contendo o grafo |
| `--rep=list|matrix` | Define o tipo de representação |
| `--stats=saida.txt` | Gera estatísticas do grafo (V, E, graus, componentes, etc.) |
| `--bfs=S --out=arquivo.txt` | Executa BFS a partir do vértice `S` e salva a árvore geradora |
| `--dfs=S --out=arquivo.txt` | Executa DFS a partir do vértice `S` e salva a árvore geradora |
| `--dist=u,v` | Calcula a distância entre os vértices `u` e `v` |
| `--diameter` | Calcula o diâmetro exato do grafo |
| `--diameter-approx=k` | Calcula o diâmetro aproximado (executa BFS a partir de `k` vértices) |
| `--bench-bfs=R` | Mede o tempo médio da BFS em `R` repetições |
| `--bench-dfs=R` | Mede o tempo médio da DFS em `R` repetições |

---

## 💻 Compilação  

Use o compilador **g++** (ou equivalente compatível com C++17):  

```bash
g++ -std=c++17 -O2 main.cpp -o grafos
```

---

## ▶️ Execução  

A seguir, alguns exemplos de uso do programa:  

### 📊 Gerar estatísticas do grafo  
```bash
./grafos --in=g1.txt --rep=list --stats=stats.txt
```

### 🌳 Executar BFS a partir do vértice 1  
```bash
./grafos --in=g1.txt --rep=list --bfs=1 --out=bfs_out.txt
```

### 🕸️ Executar DFS a partir do vértice 2  
```bash
./grafos --in=g1.txt --rep=matrix --dfs=2 --out=dfs_out.txt
```

### 📏 Calcular distância entre dois vértices  
```bash
./grafos --in=g1.txt --rep=list --dist=10,20
```

### 🧩 Calcular o diâmetro exato  
```bash
./grafos --in=g1.txt --rep=list --diameter
```

### ⚡ Calcular diâmetro aproximado (para grafos grandes)  
```bash
./grafos --in=g1.txt --rep=list --diameter-approx=5
```

### ⏱️ Executar benchmarks  
```bash
./grafos --in=g1.txt --rep=list --bench-bfs=100
./grafos --in=g1.txt --rep=matrix --bench-dfs=100
```

---

## 📚 Estudos de Caso  

O arquivo **`Estudo_de_Casos_Tabelas.xlsx`** apresenta os resultados experimentais obtidos, conforme solicitado no enunciado:  

- **Memória utilizada (MB)** para cada representação;  
- **Tempo médio de execução** de BFS e DFS;  
- **Pais e níveis** dos vértices nas árvores geradoras;  
- **Número e tamanho** das componentes conexas;  
- **Diâmetro** (exato e aproximado) de cada grafo.  

Todas as medições ignoram o tempo de leitura e escrita em disco, considerando apenas o tempo computacional efetivo dos algoritmos.  

---

## 📈 Resultados Esperados  

- **Lista de Adjacência** → melhor uso de memória e tempo em grafos esparsos.  
- **Matriz de Adjacência** → acesso direto a arestas, porém com alto consumo de memória.  
- **BFS** é usada como base para medir distâncias e o diâmetro.  
- O **diâmetro exato** é viável apenas para grafos pequenos (≤ 4000 vértices).  
- Para grafos grandes, o método **aproximado (`--diameter-approx`)** fornece resultados eficientes com erro controlado.  

---

## 🧾 Exemplo de Entrada (`g1.txt`)  

```
5
1 2
2 5
5 3
4 5
1 5
```

## 📤 Exemplo de Saída (`stats.txt`)  

```
Vertices: 5
Arestas: 5
Grau mínimo: 1
Grau máximo: 3
Grau médio: 2.0
Mediana de grau: 2
Componentes conexas: 1
Tamanho da componente: 5
```

---

## 🧍 Autor  

**Pedro Tuttman Cysne Soares**  
Curso de **Engenharia da Computação e Informação – UFRJ**  
Disciplina: **COS242 – Teoria dos Grafos**  
Ano: **2025.2**  

---

## 📄 Licença  

Este projeto é distribuído sob a licença **MIT**.  
Você pode usar, modificar e redistribuir este código livremente, desde que mantenha os devidos créditos.  

---

## 💡 Observações Finais  

- O código foi desenvolvido de forma **modular e extensível**, servindo de base para as **Partes 2 e 3** do trabalho.  
- A interface via linha de comando permite **combinar múltiplas flags** em uma única execução.  
- Logs e informações de uso de memória são impressos no **stderr**, facilitando a análise de desempenho.  
- As funções seguem boas práticas de **clareza, eficiência e reuso**, sendo ideais para integração em outros projetos.  

---

## 📘 Referências  

- **Enunciado oficial:** `trabalho-P1.pdf`  
- **Resultados experimentais:** `Estudo_de_Casos_Tabelas.xlsx`  

---

### 🌐 Contato  

📧 pedrotuttman1@gmail.com  
💻 [GitHub – Pedro Tuttman](https://github.com/PedroTuttman)  

---

> “A teoria dos grafos é a linguagem universal das conexões — de circuitos elétricos a redes sociais.”  
> — *Edsger W. Dijkstra (adaptado)*  
