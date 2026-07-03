#include "estruturas.h"

void dar_horas(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}

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
    if(atual == NULL) {
        printf("Lista vazia.\n");
        return;
    }
    while(atual != NULL) {
        // CORRIGIDO: Agora mostra a unidade corretamente
        printf("[%s] %s -> Valor: %.2f %s | Estado: %s (Sev: %d)\n", 
               atual->dados.data_hora, atual->dados.codigo, atual->dados.valor, 
               atual->dados.unidade, atual->dados.estado, atual->dados.severidade);
        atual = atual->seguinte;
    }
}

void procurar_por_codigo(NodeSensor* topo, char* cod) {
    NodeSensor* atual = topo;
    int achei = 0;
    while(atual != NULL) {
        if(strcmp(atual->dados.codigo, cod) == 0) {
            printf("\nEncontrado: %s (%s) -> %.2f %s\nMensagem: %s\n", 
                   atual->dados.codigo, atual->dados.descricao, 
                   atual->dados.valor, atual->dados.unidade, atual->dados.mensagem);
            achei = 1;
            break; 
        }
        atual = atual->seguinte;
    }
    if(achei == 0) printf("\nEsse codigo nao existe na lista.\n");
}

void guardar_binario(NodeSensor* topo) {
    FILE* f = fopen("historico.bin", "wb");
    if(f == NULL) return;
    NodeSensor* atual = topo;
    while(atual != NULL) {
        fwrite(&(atual->dados), sizeof(Sensor), 1, f);
        atual = atual->seguinte;
    }
    fclose(f);
    printf("Dados gravados em 'historico.bin'.\n");
}

// NOVO: Carregar dados guardados do passado assim que abrimos o programa
NodeSensor* carregar_binario(NodeSensor* topo) {
    FILE* f = fopen("historico.bin", "rb");
    if(f == NULL) {
        return topo; // Se nao existir ficheiro, nao faz nada
    }
    Sensor s;
    while(fread(&s, sizeof(Sensor), 1, f) == 1) {
        topo = inserir_na_lista(topo, s);
    }
    fclose(f);
    printf("Historico anterior carregado do ficheiro binario com sucesso!\n");
    return topo;
}

void criar_fila(FilaAlertas* f) {
    f->frente = NULL;
    f->retaguarda = NULL;
}

void meter_na_fila(FilaAlertas* f, Alerta a) {
    NodeAlerta* novo = (NodeAlerta*) malloc(sizeof(NodeAlerta));
    if(novo == NULL) return;
    novo->dados = a;
    novo->seguinte = NULL;
    
    if(f->retaguarda == NULL) { 
        f->frente = novo;
        f->retaguarda = novo;
    } else {
        f->retaguarda->seguinte = novo;
        f->retaguarda = novo;
    }
}

void testar_alerta_auto(FilaAlertas* f, Sensor s, int* contador) {
    if(strcmp(s.estado, "NORMAL") != 0 || s.severidade > 0) {
        static int id_contador = 1;
        Alerta novo_alerta;
        
        novo_alerta.id = id_contador++;
        dar_horas(novo_alerta.data_hora);
        strcpy(novo_alerta.sensor_codigo, s.codigo);
        strcpy(novo_alerta.descricao, s.mensagem);
        novo_alerta.prioridade = s.severidade;
        strcpy(novo_alerta.estado, "PENDENTE");
        strcpy(novo_alerta.data_hora_fim, "---");
        
        meter_na_fila(f, novo_alerta);
        *contador = *contador + 1; 
    }
}

void ver_fila_alertas(FilaAlertas f) {
    printf("\n--- FILA DE ALERTAS (FIFO) ---\n");
    NodeAlerta* atual = f.frente;
    if(atual == NULL) {
        printf("Nenhum alerta pendente.\n");
        return;
    }
    while(atual != NULL) {
        printf("ID: %d | Sensor: %s | Prioridade: %d | Estado: %s\n", 
               atual->dados.id, atual->dados.sensor_codigo, atual->dados.prioridade, atual->dados.estado);
        atual = atual->seguinte;
    }
}

void atender_alerta(FilaAlertas* f) {
    if(f->frente == NULL) {
        printf("Nada para atender na fila.\n");
        return;
    }
    NodeAlerta* aux = f->frente;
    
    printf("\nA atender Alerta ID %d (Sensor: %s)...\n", aux->dados.id, aux->dados.sensor_codigo);
    strcpy(aux->dados.estado, "CONCLUIDO");
    dar_horas(aux->dados.data_hora_fim);
    printf("Alerta fechado as %s!\n", aux->dados.data_hora_fim);
    
    f->frente = f->frente->seguinte;
    if(f->frente == NULL) {
        f->retaguarda = NULL;
    }
    free(aux); 
}

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
    printf("\n--- ULTIMA SINCRONIZACAO (TOPO DA PILHA) ---\n");
    if(topo == NULL) {
        printf("Nenhuma sincronizacao feita ainda.\n");
        return;
    }
    printf("Data/Hora: %s\n", topo->dados.data_hora);
    printf("Leituras feitas: %d\n", topo->dados.num_leituras);
    printf("Alertas criados: %d\n", topo->dados.num_alertas_gerados);
}

NodeSensor* ler_ficheiro_txt(const char* nome, NodeSensor* lista_sensores, FilaAlertas* f_alertas, NodeSinc** pilha_sinc) {
    FILE* file = fopen(nome, "r");
    if(file == NULL) {
        printf("Nao consegui abrir o ficheiro %s!\n", nome);
        return lista_sensores;
    }

    char linha[1024];
    int cont_leituras = 0;
    int cont_alertas = 0;

    // CORRIGIDO: Limpa a lista antiga da memoria para nao duplicar dados ao reler
    NodeSensor* limpa = lista_sensores;
    while(limpa != NULL) {
        NodeSensor* aux_limpa = limpa->seguinte;
        free(limpa);
        limpa = aux_limpa;
    }
    lista_sensores = NULL;

    while(fgets(linha, sizeof(linha), file) != NULL) {
        if(linha[0] == '#' || strncmp(linha, "data", 4) == 0 || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }

        Sensor s;
        int lidos = sscanf(linha, "%[^;];%[^;];%[^;];%[^;];%f;%[^;];%[^;];%d;%[^;];%[^\n]",
            s.data_hora, s.codigo, s.descricao, s.tipo, &s.valor, 
            s.unidade, s.estado, &s.severidade, s.tendencia, s.mensagem);

        if(lidos >= 8) {
            lista_sensores = inserir_na_lista(lista_sensores, s);
            cont_leituras++;
            testar_alerta_auto(f_alertas, s, &cont_alertas);
        }
    }
    fclose(file);

    *pilha_sinc = push_pilha(*pilha_sinc, cont_leituras, cont_alertas);
    printf("\nFicheiro lido! %d leituras guardadas. %d alertas gerados.\n", cont_leituras, cont_alertas);
    return lista_sensores;
}

// NOVO: Funcao obrigatoria para libertar a memoria do PC de forma limpa antes de fechar
void libertar_tudo(NodeSensor* lista, FilaAlertas* fila, NodeSinc* pilha) {
    while(lista != NULL) {
        NodeSensor* tmp = lista->seguinte;
        free(lista);
        lista = tmp;
    }
    NodeAlerta* al = fila->frente;
    while(al != NULL) {
        NodeAlerta* tmp = al->seguinte;
        free(al);
        al = tmp;
    }
    while(pilha != NULL) {
        NodeSinc* tmp = pilha->seguinte;
        free(pilha);
        pilha = tmp;
    }
}
