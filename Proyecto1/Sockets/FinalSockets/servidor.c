/*  servidor.c 
 *      Implementacion usando sockets del servidor
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

int main(int argc, char *argv[]) {

    int c, n;               // Chequeos
    int i, j;               // Iteradores
    int fila = -1;          // Numero de fila del asiento
    int columna = -1;       // Numero de columna del asiento
    int puertoS = 7000;     // Puerto por defecto del protocolo

    int fd, nuevoCliente;           // Sockets de comunicacion
    socklen_t tamCliente;           // Tamano Socket cliente
    struct sockaddr_in servDir;     // Socket nombrado del Servidor
    struct sockaddr_in clienteDir;  // Socket nombrado del Cliente

    char buffer[1024];    // Buffer para enviar y recibir
    int cbuff;          // Auxiliar para llenar el buffer

    int** vagon;   // Matriz de asientos disponibles
    int contador = 40;  // Cantidad de puestos disponibles

    
    /*
     * Manejo de parametros
     */
    opterr = 0;
    while ((c = getopt (argc, argv, "hp:f:c:")) != -1) {
        switch (c) {
            case 'h':
                printf("servidor [-f fila] [-c columna] [-p puerto de servicio]\n");
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
    if (columna < 1) {
        printf("Error parametro columna\n");
        exit(1);
    }

    if (fila < 1) {
        printf("Error: parametro fila\n");
        exit(1);
    }

    if (puertoS > 65535 || puertoS < 1025) {
        printf("Error: parametro puerto\n");
        printf("Usar puertos bien conocidos es invalido.\n");
        exit(1);
    }

    if (fila * columna > 1000) {
        printf("Tamano de vagon invalido.\n");
        printf("El vagon maximo soportado es de 1000 asientos.\n");
        exit(1);
    }


    // Matriz del vagon
    vagon = (int **) malloc(sizeof(int *) * fila);
    for (i = 0; i < fila; ++i) 
        vagon[i] = (int *) malloc(sizeof(int)*columna);


    /*
     * Inicializacion de la matriz
     */
    for (i = 0; i < fila; ++i)
        for (j = 0; j < columna; ++j)
            vagon[i][j] = 0;

    /*
     * Se crea el socket, se asocia a un puerto 
     * y comienza a escuchar por conexiones
     */
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

    listen(fd, 40); // se aceptan hasta 40 peticiones en la cola

    /*
     * Manejo de las peticiones de los clientes
     */
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

        i = atoi(strtok(buffer," ")) - 1;
        j = atoi(strtok(NULL," ")) - 1;

        memset(buffer, 0, sizeof(buffer));


        /*
         * Chequeo de asiento solicitado invalido
         */
        if (i > fila - 1 || j > columna - 1) {
            buffer[0] = '3';
            buffer[1] = (char) fila;
            buffer[2] = (char) columna; 
            n = write(nuevoCliente, buffer, 4);
            if (n < 0) {
                perror("Error: fallo de escritura en el socket");
                exit(1);
            }
            continue;
        }

        /*
         * El cliente reserva un puesto valido.
         */
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

        /*
         * El cliente reserva un puesto ocupado
         */
        } else if (contador) {
            buffer[0] = '1';
            buffer[1] = (char) fila;
            buffer[2] = (char) columna; 
            cbuff = 3;
            for (i = 0; i < fila; ++i) {
                for (j = 0; j < columna; ++j) {
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
        /*
         * El cliente reserva cuando el vagon esta lleno
         */
        } else {
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