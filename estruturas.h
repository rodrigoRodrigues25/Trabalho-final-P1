#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Dados de um sensor
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

// No para a Lista Ligada (Sensores)
typedef struct NodeSensor {
    Sensor dados;
    struct NodeSensor* seguinte;
} NodeSensor;

// Dados de um alerta
typedef struct {
    int id;
    char data_hora[30];
    char sensor_codigo[20];
    char descricao[150];
    int prioridade;
    char estado[20]; 
    char data_hora_fim[30];
} Alerta;

// No para a Fila (Alertas)
typedef struct NodeAlerta {
    Alerta dados;
    struct NodeAlerta* seguinte;
} NodeAlerta;

typedef struct {
    NodeAlerta* frente;
    NodeAlerta* retaguarda;
} FilaAlertas;

// Dados de uma sincronizacao
typedef struct {
    char data_hora[30];
    int num_leituras;
    int num_alertas_gerados;
} Sincronizacao;

// No para a Pilha (Sincronizacoes)
typedef struct NodeSinc {
    Sincronizacao dados;
    struct NodeSinc* seguinte;
} NodeSinc;

// Declaracao das funcoes para o compilador saber que elas existem
void dar_horas(char* buffer);
NodeSensor* inserir_na_lista(NodeSensor* topo, Sensor s);
void mostrar_lista(NodeSensor* topo);
void procurar_por_codigo(NodeSensor* topo, char* cod);
void guardar_binario(NodeSensor* topo);
NodeSensor* carregar_binario(NodeSensor* topo);
void criar_fila(FilaAlertas* f);
void meter_na_fila(FilaAlertas* f, Alerta a);
void testar_alerta_auto(FilaAlertas* f, Sensor s, int* contador);
void ver_fila_alertas(FilaAlertas f);
void atender_alerta(FilaAlertas* f);
NodeSinc* push_pilha(NodeSinc* topo, int l, int a);
void ver_topo_pilha(NodeSinc* topo);
NodeSensor* ler_ficheiro_txt(const char* nome, NodeSensor* lista_sensores, FilaAlertas* f_alertas, NodeSinc** pilha_sinc);
void libertar_tudo(NodeSensor* lista, FilaAlertas* fila, NodeSinc* pilha);

#endif
