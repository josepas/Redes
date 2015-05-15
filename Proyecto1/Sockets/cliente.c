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

/*
 * Especificaciones de uso del cliente
 */
void uso() {
    printf("Uso: reserva_bol_cli <ip-servidor> -f fila -c columna -p puerto \n");
    printf("\n");
    printf("-h  Muestra esta informacion de uso.\n");
    printf("-p  puerto por el cual el cliente enviara la peticion.\n");
    printf("-f  fila que el cliente desea reservar.\n");
    printf("-c  columna que el cliente desea reservar.\n");
    return;
}

/* 
 * Parametros:
 *  fila     Numero de fila del asiento a reservar
 *  columna  Numero de columna del asiento a reservar
 *
 * Retorna
 *  0 si el chequeo es correcto
 *  1 si el chequeo es incorrecto 
 */
int chequeo(int fila, int columna) {
    if (columna < 1) {
        fprintf(stderr, "Error: debe especificar una columna entre 1 y 4.\n");
        return 1;
    }

    if (fila < 1) {
        fprintf(stderr, "Error: debe especificar una fila entre 1 y 10.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    int c;               // Chequeos
    int i, j;               // Iteradores
    int fila = 0;           // Numero de Fila del asiento
    int columna = 0;        // Numero de Columna del asiento
    int puertoS = 0;        // Puerto de comunicacion con el servidor
    //int numIntentos = 3;     Cantidad de intentos de conexion con el servidor

    char* ipServidor = argv[1]; // IP del servidor
    char* sobrante;             // Chequeo

    int fd;                     // File Descriptor del socket
    struct sockaddr_in servDir; // Socket nombrado del servidor

    char buffer[1000];    // Buffer para enviar y recibir mensajes
    int cbuff;          // Auxiliar para escribir en el buffer

    int atendido;       // Chequeo de atencion de la peticion

    /*
     * Manejo de parametros
     */
    opterr = 0;
    while ((c = getopt (argc, argv, "hp:f:c:")) != -1) {
        switch (c) {
            case 'h':
                uso();
                exit(0);
            case 'p':
                puertoS = strtol(optarg,&sobrante,10); 
                if (optarg == sobrante) {
                    fprintf(stderr, "Error: el parametro puerto debe ser un entero.\n");
                    printf("\n");
                    uso();
                    exit (1);
                }
                break;
            case 'f':
                fila = strtol(optarg,&sobrante,10);
                if (optarg == sobrante) {
                    fprintf(stderr, "Error: el parametro fila debe ser un entero.\n");
                    uso();
                    exit (1);
                }
                break;
            case 'c':
                columna = strtol(optarg,&sobrante,10);
                if (optarg == sobrante) {
                    fprintf(stderr, "Error: el parametro columna debe ser un entero.\n");
                    printf("\n");
                    uso();
                    exit (1);
                }
                break;
            case '?':
                if (optopt == 'p' || optopt == 'f' || optopt == 'c')
                    fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                else
                    fprintf (stderr, "Opcion desconocida -%c.\n", optopt);
            default:
                exit(1);
        }
    }

    /*
     * Chequeo de la entrada
     */
    if (!puertoS) {
        fprintf(stderr, "Error: el parametro puerto no es opcional.\n");
        uso();
        exit(1);
    }

    if (puertoS > 65535 || puertoS < 1025) {
        fprintf(stderr, "Error: el puerto debe estar entre 1025 y 65535.\n");
        uso();
        exit(1);
    }
    
    if (chequeo(fila,columna)) {
        uso();
        exit(1);
    }


    atendido = 1;
    while (atendido) { 

        /*
         * Creacion del socket y conexion con el servidor
         */
        fd = socket(AF_INET, SOCK_STREAM, 0);
 		if (fd < 0) {
	      perror("Error: fallo en apertura del socket");
	      exit(1);
      	}

        servDir.sin_family = AF_INET;
        servDir.sin_port = htons(puertoS); 
        inet_pton(AF_INET, ipServidor, &servDir.sin_addr);

        connect(fd, (struct sockaddr *)&servDir, sizeof(servDir));

        sprintf(buffer, "%d %d\n", fila, columna);
        
        if ( write(fd, buffer, sizeof(buffer)) < 0 ) {
            perror("Error: fallo de escritura en el socket");
            exit(1);
        }

        memset(buffer, 0, sizeof(buffer));

        if (read(fd, buffer, sizeof(buffer)) < 0) {
            perror("Error: fallo de lectura en el socket");
            exit(1);
        }

        /*
         * Manejo de las respuestas del servidor
         *  Cabezera 0 reservado con exito
         *  Cabezera 1 puesto ocupado, mostrar matriz y nueva reserva
         *  Cabezera 2 vagon lleno 
         */
        if (buffer[0] == '0') {
            printf("El puesto solicitado fila:%d Columna:%d ha sido reservado con exito!\n", fila, columna);
            atendido = 0;
            
        } else if (buffer[0] == '1') {
            printf("El puesto solicitado no esta disponible.\n");
            printf("A continuacion los puestos disponibles:\n");
            cbuff = 3;
            
            printf("     1 ");
            for (i = 0; i < buffer[2]-1; i++)
                printf("%*d ", 2, i+2);
                printf("\n");    
            
            for (i = 0; i < buffer[1]; ++i) {
                printf(" %*d  ", 2, i+1);
                for (j = 0; j < buffer[2]; ++j) {
                    buffer[cbuff] == '0' ? printf("\x1b[32m" "%c  ", buffer[cbuff]) : printf("\x1b[31m" "X  ");
                    ++cbuff;
                }
                printf("\x1b[0m" "\n");
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
        
        } else if (buffer[0] == '3') {
            printf("El asiento (%d %d) solicitado es invalido.\n", fila, columna);
            printf("El vagon es de dimensiones (%dx%d).\n", buffer[1], buffer[2]);
            atendido = 0;
        }
    }

    return 0;
}
