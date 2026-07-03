#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Módulos 1 e 2: Dados do Sensor
typedef struct {
    char data_hora[30];
    char codigo[20];
    char descricao[100];
    char tipo[30];
    float valor;
    char unidade[10];
    char estado[20];
    int severidade;
    char tendencia[20];
    char mensagem[150];
} Sensor;

typedef struct NodeSensor {
    Sensor dados;
    struct NodeSensor* seguinte;
} NodeSensor;

// Módulo 3: Dados do Alerta
typedef struct {
    int id;
    char data_hora[30];
    char sensor_codigo[20];
    char descricao[150];
    int prioridade;
    char estado[20]; // PENDENTE, EM CURSO, CONCLUIDO
    char data_hora_fim[30];
} Alerta;

typedef struct NodeAlerta {
    Alerta dados;
    struct NodeAlerta* seguinte;
} NodeAlerta;

typedef struct {
    NodeAlerta* frente;
    NodeAlerta* retaguarda;
} FilaAlertas;

// Módulo 4: Dados da Sincronização
typedef struct {
    char data_hora[30];
    int num_leituras;
    int num_alertas_gerados;
} Sincronizacao;

typedef struct NodeSinc {
    Sincronizacao dados;
    struct NodeSinc* seguinte;
} NodeSinc;

// Declarações das funções
void dar_horas(char* buffer);
NodeSensor* inserir_na_lista(NodeSensor* topo, Sensor s);
void mostrar_lista(NodeSensor* topo);
void procurar_por_codigo(NodeSensor* topo, char* cod);
void listar_por_estado(NodeSensor* topo, char* est);
void listar_por_tipo(NodeSensor* topo, char* tp);
void guardar_binario(NodeSensor* topo);
NodeSensor* carregar_binario(NodeSensor* topo);

void criar_fila(FilaAlertas* f);
void meter_na_fila(FilaAlertas* f, Alerta a);
void testar_alerta_auto(FilaAlertas* f, Sensor s, int* contador);
void registar_alerta_manual(FilaAlertas* f, NodeSensor* topo);
void ver_alertas_pendentes(FilaAlertas f);
void ver_alertas_concluidos(NodeAlerta* lista_concluidos);
NodeAlerta* atender_proximo_alerta(FilaAlertas* f, NodeAlerta* lista_concluidos);

NodeSinc* push_pilha(NodeSinc* topo, int l, int a);
void ver_topo_pilha(NodeSinc* topo);
void listar_n_sincronizacoes(NodeSinc* topo, int n);
NodeSinc* pop_sincronizacao(NodeSinc* topo);

NodeSensor* ler_ficheiro_txt(const char* nome, NodeSensor* lista_sensores, FilaAlertas* f_alertas, NodeSinc** pilha_sinc);
void libertar_tudo(NodeSensor* lista, FilaAlertas* fila, NodeAlerta* concluidos, NodeSinc* pilha);

#endif
