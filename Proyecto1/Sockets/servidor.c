#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {

    int c, i, j, n;
    int fila = -1, columna = -1, puertoS = 7000;

    int fd, nuevoCliente;
    socklen_t tamCliente;
    struct sockaddr_in servDir, clienteDir;

    char buffer[50];

    int vagon[10][4];
    int contador = 40;
    int cbuff;

    for (i = 0; i < 10; ++i) 
        for (j = 0; j < 4; ++j)
            vagon[i][j] = 0;

    opterr = 0;
    while ((c = getopt (argc, argv, "hp:f:c:")) != -1) {
        switch (c) {
            case 'h':
                printf("reserva_bol_ser [-f fila] [-c columna] [-p puerto de servicio]\n");
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
    
    if (columna > 3 || columna < 0) {
        printf("Error parametro columna\n");
        exit(1);
    }

    if (fila > 9 || fila < 0) {
        printf("Error: parametro fila\n");
        exit(1);
    }

    if (puertoS > 65535 || puertoS < 1025) {
        printf("Error: parametro puerto\n");
        exit(1);
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      perror("Error: fallo en apertura del socket");
      exit(1);
    }

    servDir.sin_family = AF_INET;
    servDir.sin_addr.s_addr = INADDR_ANY;
    servDir.sin_port = htons(puertoS); 
    if (bind(fd, (struct sockaddr *)&servDir, sizeof(servDir)) < 0) {
        perror("Error: fallo en el nombramiento del socket");
        exit(1);
    }

    listen(fd, 40);

    while (1) {

        nuevoCliente = accept(fd, (struct sockaddr *) &clienteDir, &tamCliente);
        if (nuevoCliente < 0) {
            perror("Error: fallo al aceptar conexion con el cliente");
            exit(1);
        }

        n = read(nuevoCliente, buffer, sizeof(buffer));
        if (n < 0) {
            perror("Error: fallo en lectura del socket");
            exit(1);
        }

        i = atoi(strtok(buffer," "));
        j = atoi(strtok(NULL," "));

        memset(buffer, 0, sizeof(buffer));
        
        if (vagon[i][j] == 0) {
            vagon[i][j] = 1;
            buffer[0] = '0';
            buffer[1] = '\n';
            --contador;
            n = write(nuevoCliente, buffer, 2);
            if (n < 0) {
                perror("Error: fallo de escritura en el socket");
                exit(1);
            }

        } else if (contador) {
            printf("Ocupado! %d-%d\n", i, j);
            buffer[0] = '1';
            cbuff = 1;
            for (i = 0; i < 10; ++i) {
                for (j = 0; j < 4; ++j) {
                    buffer[cbuff] = (vagon[i][j] == 0) ?  '0' : '1';
                    ++cbuff;
                }
            }
            buffer[cbuff] = '\n';
            n = write(nuevoCliente, buffer, sizeof(buffer));
            if (n < 0) {
                perror("Error: fallo de escritura en el socket");
                exit(1);
            }
        } else {
            printf("Vagon lleno!\n");
            buffer[0] = '2';
            buffer[1] = '\n';
            n = write(nuevoCliente, buffer, 2);
            if (n < 0) {
                perror("Error: fallo de escritura en el socket");
                exit(1);
            }
        }
    }


    return 0;
}