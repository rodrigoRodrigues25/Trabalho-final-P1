#include "estruturas.h"

int main() {
    NodeSensor* a_minha_lista = NULL;
    FilaAlertas a_minha_fila;
    criar_fila(&a_minha_fila);
    NodeAlerta* os_meus_concluidos = NULL; // Guarda alertas resolvidos
    NodeSinc* a_minha_pilha = NULL;

    a_minha_lista = carregar_binario(a_minha_lista);

    int opcao, n_sinc;
    char filtro[50];

    do {
        printf("\n==================================================");
        printf("\n          PAINEL DE CONTROL DO DATA CENTER        ");
        printf("\n==================================================");
        printf("\n1. Importar dados ('resposta_api.txt')");
        printf("\n2. Listar todas as Leituras");
        printf("\n3. Pesquisar Sensor por Codigo");
        printf("\n4. Filtrar Sensores por Estado (NORMAL, ALERTA...)");
        printf("\n5. Filtrar Sensores por Tipo (Temperatura...)");
        printf("\n6. Registar Alerta Manualmente");
        printf("\n7. Ver Alertas Pendentes (Fila)");
        printf("\n8. Atender e Concluir Proximo Alerta");
        printf("\n9. Ver Alertas Concluidos");
        printf("\n10. Consultar Ultima Sincronizacao (Topo)");
        printf("\n11. Listar N Sincronizacoes mais Recentes");
        printf("\n12. Eliminar Ultima Sincronizacao (Pop)");
        printf("\n13. Gravar Dados (Binario) e Sair do Sistema");
        printf("\nEscolha uma opcao: ");
        
        scanf("%d", &opcao); getchar(); 

        switch(opcao) {
            case 1: a_minha_lista = ler_ficheiro_txt("resposta_api.txt", a_minha_lista, &a_minha_fila, &a_minha_pilha); break;
            case 2: mostrar_lista(a_minha_lista); break;
            case 3:
                printf("Codigo do sensor: "); fgets(filtro, sizeof(filtro), stdin); filtro[strcspn(filtro, "\n")] = 0;
                procurar_por_codigo(a_minha_lista, filtro); break;
            case 4:
                printf("Introduza o estado a filtrar: "); fgets(filtro, sizeof(filtro), stdin); filtro[strcspn(filtro, "\n")] = 0;
                listar_por_estado(a_minha_lista, filtro); break;
            case 5:
                printf("Introduza o tipo a filtrar: "); fgets(filtro, sizeof(filtro), stdin); filtro[strcspn(filtro, "\n")] = 0;
                listar_por_tipo(a_minha_lista, filtro); break;
            case 6: registar_alerta_manual(&a_minha_fila, a_minha_lista); break;
            case 7: ver_alertas_pendentes(a_minha_fila); break;
            case 8: os_meus_concluidos = atender_proximo_alerta(&a_minha_fila, os_meus_concluidos); break;
            case 9: ver_alertas_concluidos(os_meus_concluidos); break;
            case 10: ver_topo_pilha(a_minha_pilha); break;
            case 11:
                printf("Quantas sincronizacoes quer ver? "); scanf("%d", &n_sinc); getchar();
                listar_n_sincronizacoes(a_minha_pilha, n_sinc); break;
            case 12: a_minha_pilha = pop_sincronizacao(a_minha_pilha); break;
            case 13:
                guardar_binario(a_minha_lista);
                libertar_tudo(a_minha_lista, &a_minha_fila, os_meus_concluidos, a_minha_pilha);
                printf("RAM Limpa. Desligando com seguranca!\n"); break;
            default: printf("Opcao Invalida!\n");
        }
    } while(opcao != 13);

    return 0;
}
