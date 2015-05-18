/*  cliente.c 
 *      Implementacion usando sockets del cliente
 *      con modelo cliente-servidor
 * 
 *  Autores:
 *      Gustavo Gutierrez   11-10428
 *      Jose Pascarella     11-10743
 *
 *  Ultima Modificacion:
 *      8 / 05 / 2015
 */

#include <stdio.h>
#include <unistd.h>
#include "tren.h"

int chequeo(int fila, int columna) {
    if (columna > 4 || columna < 1) {
        fprintf(stderr, "Error: debe especificar una columna entre 1 y 4.\n");
        return 1;
    }

    if (fila > 10 || fila < 1) {
        fprintf(stderr, "Error: debe especificar una fila entre 1 y 10.\n");
        return 1;
    }
    return 0;
}

void uso() {
    printf("Uso: reserva_bol_cli <ip-servidor> -f fila -c columna \n");
    printf("\n");
    printf("-h  Muestra esta informacion de uso.\n");
    printf("-f  fila que el cliente desea reservar.\n");
    printf("-c  columna que el cliente desea reservar.\n");
    return;
}

int main (int argc, char *argv[]) {

    int i,j;                // Iteradores de ciclos.
    int c;                  // Chequeo de opciones
    int numIntentos = 3;    // Intentos de conexion con el servidor
    CLIENT *cl;             // Cliente de RPC
    char *host;             // IP de la máquina donde se encuentra el servidor
    char* sobrante;         // Chequeo de parametros
    respuesta *resultado;   // Resultado del procedimiento del servidor
    par asiento = {0,0};    // Asiento a reservar

    if (argc < 4) {
        uso();
        exit (1);
    }
    host = argv[1];

    /* Manejo de parametros */

    opterr = 0;
    while ((c = getopt (argc, argv, "hp:f:c:")) != -1) {
        switch (c) {
            case 'h':
                uso();
                exit(0);
            case 'f':
                asiento[0] = strtol(optarg,&sobrante,10);
                if (optarg == sobrante)
                {
                    fprintf(stderr, "Error: el parametro fila debe ser un entero.\n");
                    uso();
                    exit (1);
                }
                break;
            case 'c':
                asiento[1] = strtol(optarg,&sobrante,10);
                if (optarg == sobrante)
                {
                    fprintf(stderr, "Error: el parametro columna debe ser un entero.\n");
                    printf("\n");
                    uso();
                    exit (1);
                }
                break;
            case '?':
                if (optopt == 'f' || optopt == 'c' )
                    fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                else
                    fprintf (stderr, "Opcion desconocida -%c.\n", optopt);
            default:
                exit(1);
        }
    }

    /* Chequeo de los valores de los parametros */

    if (chequeo(asiento[0], asiento[1])) {
        uso();
        exit (1);
    }

    /* Creacion del cliente de RPC 
     * Se realizan varios intentos de conexion
     */
    i = 0;
    while (i < numIntentos) {
        cl = clnt_create(host, TREN_PROG, TREN_VERS, "tcp");
        if (cl != NULL) {
            break;
        }
        i++;
        printf("Intento fallido: %d \n",i );
        sleep(2);
    }

    if (i == numIntentos) {
           printf("Fallo de conexion con el servidor.\n");
           exit(1);
       }

    /* Llamada a la funcion y procesamiento de resultado. */

    while (1) {
        int i,j;
        resultado = reservar_1(asiento, cl);
        if (!resultado->codigo)
        /* Si el codigo es 0, el puesto fue reservado exitosamente. */
        {
            printf("El puesto ha sido reservado exitosamente.\n");
            break;
        } else if (resultado->codigo == 1)
        /* Si el codigo es 1, el puesto esta ocupado.
         * Se muestra la lista de disponibles y se pide un nuevo asiento. 
         */
        {
            printf("El puesto solicitado no esta disponible.\n");
            printf("A continuacion los puestos disponibles:\n");
            printf("     1 2 3 4\n");
            for (i = 0; i < 10; ++i)
            {
                printf(" %*d  ", 2, i+1);
                for (j = 0; j < 4; ++j)
                {
                    resultado->disponibles[i*4+j] == '0' ? printf("\x1b[32m" "0 ") : printf("\x1b[31m" "X ");
                }
                printf("\x1b[0m" "\n");
            }
            printf("Elija un nuevo asiento (fila columna): ");
            scanf("%d %d", &asiento[0], &asiento[1]);

            while (chequeo(asiento[0], asiento[1])) {
                printf("Elija un nuevo asiento (fila columna): ");
                scanf("%d %d", &asiento[0], &asiento[1]);
            }
        } else if (resultado->codigo == 2)
        /* Si el codigo es 2, el vagon esta lleno. */
        {
            printf("El tren está lleno.\n");
            break;
        }
    }
exit (0);
}
