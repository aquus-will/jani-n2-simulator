/* ========================================================================= */
/* Simulador para o processador Nios II da Altera
   Arquivo com fun��es de interpreta��o (headers)
   Cria��o: 20-ago-2007
   Criado por: Willian dos Santos Lima */
/* ========================================================================= */

#ifndef INTSIM
#define INTSIM

#include "def_sim.h"

/* interpreta��o de instru��o I-Type*/
char int_itype ( u4 word, u4 *pc );

/* interpreta��o de instru��o R-Type*/
char int_rtype ( u4 word, u4 *pc );

/* interpreta��o de instru��o J-Type*/
char int_jtype ( u4 word, u4 *pc );

#endif
