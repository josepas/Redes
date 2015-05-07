/* tren.x: Protocolo de reserva de puestos remoto */

typedef int par[2];
struct respuesta{
	int codigo;
	char disponibles[40];
};

program TREN_PROG {
	version TREN_VERS {
		respuesta RESERVAR(par)=1;
	}= 1 ;
}= 0x200000001;