#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    int c;
    int fila = -1, columna = -1, puertoS = -1;
    char* ipServidor = argv[1];

 
    opterr = 0;
    while ((c = getopt (argc, argv, "hp:f:c:")) != -1) {
        switch (c) {
            case 'h':
                printf("reserva_bol_cli <ip-servidor> [-p puerto de servicio] [-f fila] [-c columna]\n");
                printf("\n");
                printf("-h  Muestra esta informacion de uso.\n");
                printf("-p  puerto por el cual el cliente enviara la peticion.\n");
                printf("-f  fila que el cliente desea reservar.\n");
                printf("-c  columna que el cliente desea reservar.\n");
                exit(0);
            case 'p':
                puertoS = atoi(optarg);
                break;
            case 'f':
                fila = atoi(optarg) - 1;
                break;
            case 'c':
                columna = atoi(optarg) - 1;
                break;
            case '?':
                if (optopt == 'p')
                    fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                else if (optopt == 'f')
                    fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                else if (optopt == 'c')
                    fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                else
                    fprintf (stderr, "Opcion desconocida -%c.\n", optopt);
            default:
                exit(1);
        }
    }
    //salida = argv[optind];

    if (columna > 3 || columna < 0) {
        printf("Error parametro columna\n");
        exit(1);
    }

    if (fila > 9 || fila < 0) {
        printf("Error parametro fila\n");
        exit(1);
    }

    if (puertoS > 65535 || puertoS < 1025) {
        printf("Error parametro puerto\n");
        exit(1);
    }

    // FLAG
    printf("ipServidor: %s. puerto: %d. fila: %d. columna: %d\n", ipServidor, puertoS, fila, columna);
    return 0;
}
