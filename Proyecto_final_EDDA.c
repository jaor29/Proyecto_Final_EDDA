#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAJA 1
#define MEDIA 2
#define ALTA 3

#define MAX_DESC 100

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

typedef struct Tarea {
    char descripcion[MAX_DESC];
    int prioridad;
    struct Tarea *sig;
} Tarea;

typedef struct NodoCompletada {
    char descripcion[MAX_DESC];
    int prioridad;
    struct NodoCompletada *sig;
} NodoCompletada;

Tarea *pendientes = NULL;
NodoCompletada *completadas = NULL;

void agregarTarea(char *desc, int prioridad) {
    Tarea *nueva = (Tarea *)malloc(sizeof(Tarea));
    strcpy(nueva->descripcion, desc);
    nueva->prioridad = prioridad;
    nueva->sig = NULL;

    if (!pendientes || prioridad > pendientes->prioridad) {
        nueva->sig = pendientes;
        pendientes = nueva;
        return;
    }

    Tarea *actual = pendientes;
    while (actual->sig && actual->sig->prioridad >= prioridad) {
        actual = actual->sig;
    }
    nueva->sig = actual->sig;
    actual->sig = nueva;
}

void listarPendientes() {
    printf("\nTareas pendientes:\n");
    Tarea *actual = pendientes;
    while (actual) {
        printf("- [%s] Prioridad: %s\n", actual->descripcion,
               actual->prioridad == ALTA ? "Alta" :
               actual->prioridad == MEDIA ? "Media" : "Baja");
        actual = actual->sig;
    }
}

void marcarCompletadaPorIndice(int indice) {
    if (!pendientes) {
        printf("No hay tareas pendientes.\n");
        return;
    }
    Tarea *actual = pendientes, *anterior = NULL;
    int i = 1;
    while (actual && i < indice) {
        anterior = actual;
        actual = actual->sig;
        i++;
    }
    if (!actual) {
        printf("Indice invalido.\n");
        return;
    }

    char desc[MAX_DESC];
    int prioridad = actual->prioridad;
    strcpy(desc, actual->descripcion);

    if (!anterior) pendientes = actual->sig;
    else anterior->sig = actual->sig;

    NodoCompletada *nodo = (NodoCompletada *)malloc(sizeof(NodoCompletada));
    strcpy(nodo->descripcion, desc);
    nodo->prioridad = prioridad;
    nodo->sig = NULL;

    if (!completadas) completadas = nodo;
    else {
        NodoCompletada *aux = completadas;
        while (aux->sig) aux = aux->sig;
        aux->sig = nodo;
    }

    free(actual);
    printf("Tarea '%s' marcada como completada.\n", desc);
}

void mostrarCompletadas() {
    printf("\nTareas completadas:\n");
    NodoCompletada *actual = completadas;
    while (actual) {
        printf("- %s\n", actual->descripcion);
        actual = actual->sig;
    }
}

void buscarTarea(char *desc) {
    Tarea *p = pendientes;
    NodoCompletada *c = completadas;
    int encontrada = 0;

    printf("\nResultado de la busqueda:\n");
    while (p) {
        if (strstr(p->descripcion, desc)) {
            printf("Pendiente: %s\n", p->descripcion);
            encontrada = 1;
        }
        p = p->sig;
    }
    while (c) {
        if (strstr(c->descripcion, desc)) {
            printf("Completada: %s\n", c->descripcion);
            encontrada = 1;
        }
        c = c->sig;
    }
    if (!encontrada) printf("No se encontro ninguna coincidencia.\n");
}

void guardarTareas() {
    FILE *fp = fopen("pendientes.txt", "w");
    Tarea *p = pendientes;
    while (p) {
        fprintf(fp, "%d|%s\n", p->prioridad, p->descripcion);
        p = p->sig;
    }
    fclose(fp);

    fp = fopen("completadas.txt", "w");
    NodoCompletada *c = completadas;
    while (c) {
        fprintf(fp, "%d|%s\n", c->prioridad, c->descripcion);
        c = c->sig;
    }
    fclose(fp);
}

void cargarTareas() {
    FILE *fp = fopen("pendientes.txt", "r");
    if (fp) {
        char linea[200];
        while (fgets(linea, sizeof(linea), fp)) {
            int prioridad;
            char desc[MAX_DESC];
            char *sep = strchr(linea, '|');
            if (sep) {
                *sep = '\0';
                prioridad = atoi(linea);
                strcpy(desc, sep + 1);
                desc[strcspn(desc, "\n")] = '\0';
                agregarTarea(desc, prioridad);
            }
        }
        fclose(fp);
    }

    fp = fopen("completadas.txt", "r");
    if (fp) {
        char linea[200];
        while (fgets(linea, sizeof(linea), fp)) {
            int prioridad;
            char desc[MAX_DESC];
            char *sep = strchr(linea, '|');
            if (sep) {
                *sep = '\0';
                prioridad = atoi(linea);
                strcpy(desc, sep + 1);
                desc[strcspn(desc, "\n")] = '\0';
                NodoCompletada *nodo = (NodoCompletada *)malloc(sizeof(NodoCompletada));
                strcpy(nodo->descripcion, desc);
                nodo->prioridad = prioridad;
                nodo->sig = NULL;
                if (!completadas) completadas = nodo;
                else {
                    NodoCompletada *aux = completadas;
                    while (aux->sig) aux = aux->sig;
                    aux->sig = nodo;
                }
            }
        }
        fclose(fp);
    }
}



int main() {
    int opcion;
    char descripcion[MAX_DESC];
    int prioridad;

    cargarTareas();

    do {
        system(CLEAR);
        printf("\nSimulador de Gestion de Tareas\n");
        printf("1. Agregar tarea\n");
        printf("2. Listar tareas pendientes\n");
        printf("3. Marcar tarea como completada\n");
        printf("4. Mostrar tareas completadas\n");
        printf("5. Buscar tarea\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer

        system(CLEAR);
        switch (opcion) {
            case 1:
                printf("Descripcion: ");
                fgets(descripcion, MAX_DESC, stdin);
                descripcion[strcspn(descripcion, "\n")] = '\0';
                printf("Prioridad (1=Baja, 2=Media, 3=Alta): ");
                scanf("%d", &prioridad);
                getchar();
                agregarTarea(descripcion, prioridad);
                break;
            case 2:
                listarPendientes();
                break;
            case 3:{
                int total = 0;
                Tarea *tmp = pendientes;
                printf("Tareas pendientes:\n");
                while (tmp) {
                    printf("%d. [%s] Prioridad: %s\n", ++total, tmp->descripcion,
                        tmp->prioridad == ALTA ? "Alta" :
                        tmp->prioridad == MEDIA ? "Media" : "Baja");
                    tmp = tmp->sig;
                }
                if (total == 0) {
                    printf("No hay tareas pendientes.\n");
                    break;
                }
                int seleccion;
                printf("Seleccione el indice de la tarea a completar: ");
                scanf("%d", &seleccion);
                getchar();
                marcarCompletadaPorIndice(seleccion);
                break;
            }
                
            case 4:
                mostrarCompletadas();
                break;
            case 5:
                printf("Termino de busqueda: ");
                fgets(descripcion, MAX_DESC, stdin);
                descripcion[strcspn(descripcion, "\n")] = '\0';
                buscarTarea(descripcion);
                break;
            case 0:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
        if (opcion != 0) {
            printf("\nPresione Enter para continuar...");
            getchar();
        }
    } while (opcion != 0);

    guardarTareas();

    while (pendientes) {
        Tarea *tmp = pendientes;
        pendientes = pendientes->sig;
        free(tmp);
    }

    while (completadas) {
        NodoCompletada *tmp = completadas;
        completadas = completadas->sig;
        free(tmp);
    }

    return 0;
}
