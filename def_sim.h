/* ========================================================================= */
/* Simulador para o processador Nios II da Altera
   Arquivo com defini��es
   Cria��o: 16-ago-2007
   Modificado: 28-ago-2007
   Criado por: Willian dos Santos Lima */
/* ========================================================================= */

#ifndef DEF_SIM
#define DEF_SIM

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* tipos sem sinal*/
#define u1 unsigned char
#define u2 unsigned short
#define u4 unsigned long
#define u8 unsigned long long

/* tipos com sinal*/
#define s1 char
#define s2 short
#define s4 long
#define s8 long long

/* registradores de prop�sito geral*/
#define zero 0
#define at 1
#define et 24
#define bt 25
#define gp 26
#define sp 27
#define fp 28
#define ea 29
#define ba 30
#define ra 31

/* registradores de controle*/
#define status 0
#define estatus 1
#define bstatus 2
#define ienable 3
#define ipending 4
#define cpuid 5

/* globais declaradas em 'int_sim.c'*/
/* declara��o do array de registradores e do controlador de qual reg. est�
   dispon�vel para aloca��o de vari�veis*/
extern s4 reg[32];    /* 0..31*/
extern u4 block;      /* 31..0 (bits)*/

extern s4 ctrl[6];   /* registradores de controle*/

/* minha mem�ria RAM*/
extern u1 *ram;

/* bits de controle*/
#define U_bit (ctrl[status] & 2)
#define PIE_bit (ctrl[status] & 1)
#define EU_bit (ctrl[estatus] & 2)
#define EPIE_bit (ctrl[estatus] & 1)
#define BU_bit (ctrl[bstatus] & 2)
#define BPIE_bit (ctrl[bstatus] & 1)

/* fun��o para checar se um registrador est� dispon�vel ou n�o*/
#define is_block(A) ((1<<A) & block)

/* ========================================================================= */
/* vari�veis globais em 'n2run'*/
extern u4 GLOBAL_size;

extern u4 GLOBAL_n_add, GLOBAL_n_sub, GLOBAL_n_mul, GLOBAL_n_div;
extern u4 GLOBAL_n_logic, GLOBAL_n_relat, GLOBAL_n_rotat;
extern u4 GLOBAL_n_load, GLOBAL_n_store, GLOBAL_n_branch, GLOBAL_n_others;

#endif
