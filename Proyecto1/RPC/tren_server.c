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

#include "tren.h"

respuesta * reservar_1_svc(int *argp, struct svc_req *rqstp)
{
	int i,j; 							// Iteradores de ciclos.
	int f,c;							// Fila y Columna indexados desde 0.
	static respuesta  result;			// Resultado del procedimiento.
	static int vagon [10][4] = {0};		// Matriz de asientos. (0 = Disponible, 1 = Ocupado)
	static int cont = 40;				// Contador de asientos disponibles.
	f = argp[0] -1;						
	c = argp[1] -1;

	if (!vagon[f][c])
	/* Si el asiento pedido esta disponible lo reservamos. */
	{
		vagon[f][c] = 1;
		cont--;
		result.codigo = 0;
	} else if (cont)
	/* Si quedan asientos disponibles enviamos los disponibles. */
	{
		result.codigo = 1;
		for (i = 0; i < 10; ++i)
			for (j = 0; j < 4; ++j)
				result.disponibles[i*4+j] = vagon[i][j] ? 'X' : '0'; 
	} else {
	/* Si no quedan asientos enviamos el codigo correspondiente. */
		result.codigo = 2;
	}

	return &result;
}
