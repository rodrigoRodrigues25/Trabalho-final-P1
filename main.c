#include "estruturas.h"

int main() {
    NodeSensor* a_minha_lista = NULL;
    FilaAlertas a_minha_fila;
    criar_fila(&a_minha_fila);
    NodeSinc* a_minha_pilha = NULL;

    // Tenta carregar dados binarios antigos logo ao iniciar a aplicacao
    a_minha_lista = carregar_binario(a_minha_lista);

    int opcao;
    char procura[20];

    do {
        printf("\n=================================");
        printf("\n MENU MONITORIZACAO - DC IPVC");
        printf("\n=================================");
        printf("\n1. Importar 'resposta_api.txt'");
        printf("\n2. Mostrar Leituras (Lista)");
        printf("\n3. Procurar por Codigo");
        printf("\n4. Ver Alertas Pendentes (Fila)");
        printf("\n5. Resolver Proximo Alerta (FIFO)");
        printf("\n6. Ver Ultima Sincronizacao (Pilha)");
        printf("\n7. Gravar (Binario) e Sair");
        printf("\nEscolha uma opcao: ");
        
        scanf("%d", &opcao);
        getchar(); // Limpa o buffer do ENTER

        switch(opcao) {
            case 1:
                a_minha_lista = ler_ficheiro_txt("resposta_api.txt", a_minha_lista, &a_minha_fila, &a_minha_pilha);
                break;
            case 2:
                mostrar_lista(a_minha_lista);
                break;
            case 3:
                printf("Codigo do sensor (ex: TEMP_RACK): ");
                fgets(procura, sizeof(procura), stdin);
                procura[strcspn(procura, "\n")] = 0; 
                procurar_por_codigo(a_minha_lista, procura);
                break;
            case 4:
                ver_fila_alertas(a_minha_fila);
                break;
            case 5:
                atender_alerta(&a_minha_fila);
                break;
            case 6:
                ver_topo_pilha(a_minha_pilha);
                break;
            case 7:
                guardar_binario(a_minha_lista);
                libertar_tudo(a_minha_lista, &a_minha_fila, a_minha_pilha); // Limpa a memoria RAM
                printf("Fim do programa. Sistema limpo da RAM!\n");
                break;
            default:
                printf("Opcao Errada!\n");
        }
    } while(opcao != 7);

    return 0;
}
