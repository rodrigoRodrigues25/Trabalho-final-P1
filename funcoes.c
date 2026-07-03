#include "estruturas.h"

void dar_horas(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// --- MÓDULO 2: LISTA LIGADA ---
NodeSensor* inserir_na_lista(NodeSensor* topo, Sensor s) {
    NodeSensor* novo = (NodeSensor*) malloc(sizeof(NodeSensor));
    if(novo == NULL) return topo;
    novo->dados = s;
    novo->seguinte = topo; 
    return novo;           
}

void mostrar_lista(NodeSensor* topo) {
    printf("\n--- LEITURAS EM MEMORIA ---\n");
    NodeSensor* atual = topo;
    if(atual == NULL) { printf("Lista vazia.\n"); return; }
    while(atual != NULL) {
        printf("[%s] %s -> %.2f %s | Estado: %s (Sev: %d)\n", 
               atual->dados.data_hora, atual->dados.codigo, atual->dados.valor, 
               atual->dados.unidade, atual->dados.estado, atual->dados.severidade);
        atual = atual->seguinte;
    }
}

void procurar_por_codigo(NodeSensor* topo, char* cod) {
    NodeSensor* atual = topo;
    int achei = 0;
    while(atual != NULL) {
        if(strcasecmp(atual->dados.codigo, cod) == 0) {
            printf("\n[Encontrado] %s (%s) -> %.2f %s\nEstado: %s | Mensagem: %s\n", 
                   atual->dados.codigo, atual->dados.descricao, atual->dados.valor, 
                   atual->dados.unidade, atual->dados.estado, atual->dados.mensagem);
            achei = 1; break;
        }
        atual = atual->seguinte;
    }
    if(!achei) printf("\nSensor '%s' nao encontrado.\n", cod);
}

void listar_por_estado(NodeSensor* topo, char* est) {
    NodeSensor* atual = topo;
    int cont = 0;
    printf("\n--- SENSORES COM ESTADO: %s ---\n", est);
    while(atual != NULL) {
        if(strcasecmp(atual->dados.estado, est) == 0) {
            printf("%s | %s -> %.2f %s\n", atual->dados.codigo, atual->dados.descricao, atual->dados.valor, atual->dados.unidade);
            cont++;
        }
        atual = atual->seguinte;
    }
    if(cont == 0) printf("Nenhum sensor encontrado com esse estado.\n");
}

void listar_por_tipo(NodeSensor* topo, char* tp) {
    NodeSensor* atual = topo;
    int cont = 0;
    printf("\n--- SENSORES DO TIPO: %s ---\n", tp);
    while(atual != NULL) {
        if(strcasecmp(atual->dados.tipo, tp) == 0) {
            printf("%s | %s -> %.2f %s [Estado: %s]\n", atual->dados.codigo, atual->dados.descricao, atual->dados.valor, atual->dados.unidade, atual->dados.estado);
            cont++;
        }
        atual = atual->seguinte;
    }
    if(cont == 0) printf("Nenhum sensor encontrado deste tipo.\n");
}

void guardar_binario(NodeSensor* topo) {
    FILE* f = fopen("historico.bin", "wb");
    if(f == NULL) return;
    NodeSensor* @atual = topo;
    while(atual != NULL) {
        fwrite(&(atual->dados), sizeof(Sensor), 1, f);
        atual = @atual->seguinte;
    }
    fclose(f);
    printf("Dados gravados em 'historico.bin'.\n");
}

NodeSensor* carregar_binario(NodeSensor* topo) {
    FILE* f = fopen("historico.bin", "rb");
    if(f == NULL) return topo;
    Sensor s;
    while(fread(&s, sizeof(Sensor), 1, f) == 1) {
        topo = inserir_na_lista(topo, s);
    }
    fclose(f);
    printf("Historico anterior carregado do ficheiro binario!\n");
    return topo;
}

// --- MÓDULO 3: FILA (ALERTAS) ---
void criar_fila(FilaAlertas* f) {
    f->frente = NULL; f->retaguarda = NULL;
}

void meter_na_fila(FilaAlertas* f, Alerta a) {
    NodeAlerta* novo = (NodeAlerta*) malloc(sizeof(NodeAlerta));
    if(novo == NULL) return;
    novo->dados = a;
    novo->seguinte = NULL;
    if(f->retaguarda == NULL) { 
        f->frente = novo; f->retaguarda = novo;
    } else {
        f->retaguarda->seguinte = novo; f->retaguarda = novo;
    }
}

void testar_alerta_auto(FilaAlertas* f, Sensor s, int* contador) {
    if(strcmp(s.estado, "NORMAL") != 0 || s.severidade > 0) {
        static int id_auto = 1;
        Alerta a;
        a.id = id_auto++;
        dar_horas(a.data_hora);
        strcpy(a.sensor_codigo, s.codigo);
        strcpy(a.descricao, s.mensagem);
        a.prioridade = s.severidade;
        strcpy(a.estado, "PENDENTE");
        strcpy(a.data_hora_fim, "---");
        meter_na_fila(f, a);
        *contador = *contador + 1;
    }
}

void registar_alerta_manual(FilaAlertas* f, NodeSensor* topo) {
    static int id_manual = 500; 
    char cod[20];
    printf("Introduza o codigo do sensor para o alerta manual: ");
    fgets(cod, sizeof(cod), stdin);
    cod[strcspn(cod, "\n")] = 0;

    Alerta a;
    a.id = id_manual++;
    dar_horas(a.data_hora);
    strcpy(a.sensor_codigo, cod);
    printf("Descricao da anomalia manual: ");
    fgets(a.descricao, sizeof(a.descricao), stdin);
    a.descricao[strcspn(a.descricao, "\n")] = 0;
    printf("Introduza a prioridade (0 a 4): ");
    scanf("%d", &a.prioridade); getchar();
    strcpy(a.estado, "PENDENTE");
    strcpy(a.data_hora_fim, "---");

    meter_na_fila(f, a);
    printf("Alerta manual ID %d registado com sucesso na fila!\n", a.id);
}

void ver_alertas_pendentes(FilaAlertas f) {
    printf("\n--- ALERTAS PENDENTES NA FILA ---\n");
    NodeAlerta* atual = f.frente;
    
    while(atual != NULL) {
        printf("ID: %d | Sensor: %s | Prioridade: %d | Estado: %s | Desc: %s\n", 
               atual->dados.id, atual->dados.sensor_codigo, atual->dados.prioridade, atual->dados.estado, atual->dados.descricao);
        atual = atual->seguinte;
    }
}

NodeAlerta* atender_proximo_alerta(FilaAlertas* f, NodeAlerta* lista_concluidos) {
    if(f->frente == NULL) { printf("Fila de atendimento vazia.\n"); return lista_concluidos; }
    
    NodeAlerta* aux = f->frente;
    strcpy(aux->dados.estado, "EM CURSO");
    printf("\nAlerta ID %d passou para [EM CURSO]. A processar...\n", aux->dados.id);
    
    strcpy(aux->dados.estado, "CONCLUIDO");
    dar_horas(aux->dados.data_hora_fim);
    printf("Alerta ID %d Resolvido e Concluido as %s!\n", aux->dados.id, aux->dados.data_hora_fim);
    
    f->frente = f->frente->seguinte;
    if(f->frente == NULL) f->retaguarda = NULL;
    
    aux->seguinte = lista_concluidos;
    return aux; 
}

void ver_alertas_concluidos(NodeAlerta* lista_concluidos) {
    printf("\n--- HISTORICO DE ALERTAS CONCLUIDOS ---\n");
    NodeAlerta* atual = lista_concluidos;
    if(atual == NULL) { printf("Nenhum alerta concluido ainda.\n"); return; }
    while(atual != NULL) {
        printf("ID: %d | Sensor: %s | Fechado em: %s | Desc: %s\n", 
               atual->dados.id, atual->dados.sensor_codigo, atual->dados.data_hora_fim, atual->dados.descricao);
        atual = atual->seguinte;
    }
}

// --- MÓDULO 4: PILHA ---
NodeSinc* push_pilha(NodeSinc* topo, int l, int a) {
    NodeSinc* novo = (NodeSinc*) malloc(sizeof(NodeSinc));
    if(novo == NULL) return topo;
    dar_horas(novo->dados.data_hora);
    novo->dados.num_leituras = l;
    novo->dados.num_alertas_gerados = a;
    novo->seguinte = topo;
    return novo; 
}

void ver_topo_pilha(NodeSinc* topo) {
    printf("\n--- ULTIMA SINCRONIZACAO (TOPO) ---\n");
    if(topo == NULL) { printf("Nenhuma sincronizacao registada.\n"); return; }
    printf("Data: %s | Leituras: %d | Alertas criados: %d\n", topo->dados.data_hora, topo->dados.num_leituras, topo->dados.num_alertas_gerados);
}

void listar_n_sincronizacoes(NodeSinc* topo, int n) {
    printf("\n--- AS %d SINCRONIZACOES MAIS RECENTES ---\n", n);
    NodeSinc* atual = topo;
    int i = 0;
    while(atual != NULL && i < n) {
        printf("[%d] Hora: %s | Leituras: %d | Alertas: %d\n", i+1, atual->dados.data_hora, atual->dados.num_leituras, atual->dados.num_alertas_gerados);
        atual = atual->seguinte;
        i++;
    }
    if(i == 0) printf("Nenhum registo na pilha.\n");
}

NodeSinc* pop_sincronizacao(NodeSinc* topo) {
    if(topo == NULL) { printf("Pilha vazia. Nada para remover.\n"); return NULL; }
    NodeSinc* aux = topo;
    topo = topo->seguinte;
    printf("Sincronizacao de %s removida do historico.\n", aux->dados.data_hora);
    free(aux);
    return topo;
}

// --- MÓDULO 1: PARSING TXT ---
NodeSensor* ler_ficheiro_txt(const char* nome, NodeSensor* lista_sensores, FilaAlertas* f_alertas, NodeSinc** pilha_sinc) {
    FILE* file = fopen(nome, "r");
    if(file == NULL) { printf("Ficheiro %s nao encontrado!\n", nome); return lista_sensores; }
    char linha[1024];
    int cont_leituras = 0, cont_alertas = 0;

    while(lista_sensores != NULL) {
        NodeSensor* aux = lista_sensores->seguinte; free(lista_sensores); lista_sensores = aux;
    }

    while(fgets(linha, sizeof(linha), file) != NULL) {
        if(linha[0] == '#' || strncmp(linha, "data", 4) == 0 || linha[0] == '\n' || linha[0] == '\r') continue;
        Sensor s;
        int lidos = sscanf(linha, "%[^;];%[^;];%[^;];%[^;];%f;%[^;];%[^;];%d;%[^;];%[^\n]",
            s.data_hora, s.codigo, s.descricao, s.tipo, &s.valor, s.unidade, s.estado, &s.severidade, s.tendencia, s.mensagem);
        if(lidos >= 8) {
            lista_sensores = inserir_na_lista(lista_sensores, s);
            cont_leituras++;
            testar_alerta_auto(f_alertas, s, &cont_alertas);
        }
    }
    fclose(file);
    *pilha_sinc = push_pilha(*pilha_sinc, cont_leituras, cont_alertas);
    printf("\n[Sucesso] Ficheiro lido! %d leituras em memoria e %d alertas na fila.\n", cont_leituras, cont_alertas);
    return lista_sensores;
}

void libertar_tudo(NodeSensor* lista, FilaAlertas* fila, NodeAlerta* concluidos, NodeSinc* pilha) {
    while(lista != NULL) { NodeSensor* t = lista->seguinte; free(lista); lista = t; }
    NodeAlerta* al = fila->frente;
    while(al != NULL) { NodeAlerta* t = al->seguinte; free(al); al = t; }
    while(concluidos != NULL) { NodeAlerta* t = concluidos->seguinte; free(concluidos); concluidos = t; }
    while(pilha != NULL) { NodeSinc* t = pilha->seguinte; free(pilha); pilha = t; }
}
