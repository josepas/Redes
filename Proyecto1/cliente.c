#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int chequeo(int fila, int columna) {
    if (columna > 4 || columna < 1) {
        printf("Error parametro columna\n");
        printf("%d\n", columna );
        return 1;
    }

    if (fila > 10 || fila < 1) {
        printf("Error parametro fila\n");
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]) {

    int c, i, j;
    int fila = -1, columna = -1, puertoS = -1;
    char* ipServidor = argv[1];

    struct sockaddr_in servDir;
    int fd;

    char buffer[50];
    int cbuff;

    int atendido;


 
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
                fila = atoi(optarg);
                break;
            case 'c':
                columna = atoi(optarg);
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

    if (puertoS > 65535 || puertoS < 1025) {
            printf("Error parametro puerto\n");
            exit(1);
    }

    atendido = 1;
    while (atendido) { 

        if (chequeo(fila,columna)) exit(1);

        fd = socket(AF_INET, SOCK_STREAM, 0);

        servDir.sin_family = AF_INET;
        servDir.sin_port = htons(puertoS); 
        inet_pton(AF_INET, ipServidor, &servDir.sin_addr);

        connect(fd, (struct sockaddr *)&servDir, sizeof(servDir));

        sprintf(buffer, "%d %d\n", fila-1, columna-1);
        write(fd, buffer, sizeof(buffer));

        memset(buffer, 0, sizeof(buffer));

        read(fd, buffer, sizeof(buffer));

        if (buffer[0] == '0') {
            printf("El puesto solicitado fila:%d Columna:%d ha sido reservado con exito!\n", fila, columna);
            atendido = 0;
            
        } else if (buffer[0] == '1') {
            printf("El puesto solicitado no esta disponible.\n");
            printf("Acontinuacion, los puestos disponibles:\n");
            cbuff = 1;
            printf("     1 2 3 4\n");
            for (i = 0; i < 10; ++i) {
                printf(" %*d  ", 2, i+1);
                for (j = 0; j < 4; ++j) {
                    buffer[cbuff] == '0' ? printf("\x1b[32m" "%c ", buffer[cbuff]) : printf("\x1b[31m" "X ");
                    ++cbuff;
                }
                printf("\n");
            }

            printf("Elija un nuevo asiento (fila columna): ");
            scanf("%d %d", &fila, &columna);

            while (chequeo(fila, columna)) {
                printf("Elija un nuevo asiento (fila columna): ");
                scanf("%d %d", &fila, &columna);
            }


            close(fd);
            

        } else if (buffer[0] == '2') {
            printf("El vagon se encuntra lleno, intente en otro viaje.\n");
            atendido = 0;
        }
    }

    return 0;
}
