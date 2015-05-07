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

    int c, i, j;
    int fila = -1, columna = -1, puertoS = 7000;

    int fd, nuevoCliente;
    socklen_t tamCliente;
    struct sockaddr_in servDir, clienteDir;

    char buffer[50];
    //int* nbuff;
    //char* aux;

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
        printf("Error parametro fila\n");
        exit(1);
    }

    if (puertoS > 65535 || puertoS < 1025) {
        printf("Error parametro puerto\n");
        exit(1);
    }

    // FLAG
    printf("Puerto: %d. fila: %d. columna: %d\n", puertoS, fila, columna);

    // crear el socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    //printf("%d\n", fd);

    // "Darle un nombre al socket
    servDir.sin_family = AF_INET;
    servDir.sin_addr.s_addr = INADDR_ANY;
    servDir.sin_port = htons(puertoS); 
    bind(fd, (struct sockaddr *)&servDir, sizeof(servDir));

    // Escuchar por las conexiones
    // parametro es la cantidad de peticiones que se pueden encolar
    listen(fd, 40);

    while (1) {

        nuevoCliente = accept(fd, (struct sockaddr *) &clienteDir, &tamCliente);

        read(nuevoCliente, buffer, sizeof(buffer));

        i = atoi(strtok(buffer," "));
        printf("%d--", i);
        j = atoi(strtok(NULL," "));
        printf("%d--\n", j);

        memset(buffer, 0, sizeof(buffer));
        
        if (vagon[i][j] == 0) {
            printf("reservado!\n");
            vagon[i][j] = 1;
            buffer[0] = '0';
            buffer[1] = '\n';
            --contador;
            write(nuevoCliente, buffer, 2);

        } else if (contador) {
            printf("Ocupado!\n");
            buffer[0] = '1';
            cbuff = 1;
            for (i = 0; i < 10; ++i) {
                for (j = 0; j < 4; ++j) {
                    buffer[cbuff] = (vagon[i][j] == 0) ?  '0' : '1';
                    ++cbuff;
                }
            }
            buffer[cbuff] = '\n';
            write(nuevoCliente, buffer, sizeof(buffer));

        } else {
            printf("Lleno!\n");
            buffer[0] = '2';
            buffer[1] = '\n';
            write(nuevoCliente, buffer, 2);
        }
    }


















    return 0;
}