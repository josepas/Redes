/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _TREN_H_RPCGEN
#define _TREN_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef int par[2];

struct respuesta {
	int codigo;
	char disponibles[40];
};
typedef struct respuesta respuesta;

#define TREN_PROG 0x20000001
#define TREN_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define RESERVAR 1
extern  respuesta * reservar_1(int *, CLIENT *);
extern  respuesta * reservar_1_svc(int *, struct svc_req *);
extern int tren_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define RESERVAR 1
extern  respuesta * reservar_1();
extern  respuesta * reservar_1_svc();
extern int tren_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_par (XDR *, par);
extern  bool_t xdr_respuesta (XDR *, respuesta*);

#else /* K&R C */
extern bool_t xdr_par ();
extern bool_t xdr_respuesta ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_TREN_H_RPCGEN */
