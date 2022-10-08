/* ========================================================================= */
/* Simulador para o processador Nios II da Altera
   Arquivo com fun��es de interpreta��o (headers)
   Cria�ao: 15-ago-2007
   Modificado: 07-jan-2008
   Criado por: Willian dos Santos Lima */
/* ========================================================================= */

#include "int_sim.h"

/* declara��o do array de registradores e do controlador de qual reg. est�
   dispon�vel para aloca��o de vari�veis*/
s4 reg[32];    /* 0..31*/
u4 block;      /* 31..0 (bits)*/

s4 ctrl[6];   /* registradores de controle*/

/* minha mem�ria RAM*/
u1 *ram;

/* interpreta��o de instru��o I-Type*/
char int_itype ( u4 word, u4 *pc ){
   u4 op;
   u4 a, b;
   s4 imm16, aux;
   s8 tmp_s8;
   
   op = word << 26;
   op = op >> 26;
   
   imm16 = word << 10;
   imm16 = imm16 >> (10+6);
   
   a = word >> 27;
   b = ( word << 5 ) >> 27;
   
   /*printf("\n---\nDEBUG:\n");
   printf("op = %X\timm16 = %d\nrA = %u, rB = %u\n---\n",op,imm16,a,b);*/
   
   switch ( op ){
      /* instru��es l�gicas de compara��o*/
      case 0x08:  /* [cmpgei]: instru��o gran equal*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ( reg[a] >= (s2) imm16 );
                  GLOBAL_n_relat++;
                  break;
      case 0x10:  /* [cmplti]: instru��o less*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ( reg[a] < (s2) imm16 );
                  GLOBAL_n_relat++;
                  break;
      case 0x18:  /* [cmpnei]: instru��o not equal*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ( reg[a] != (s2) imm16 );
                  GLOBAL_n_relat++;
                  break;
      case 0x20:  /* [cmpeqi]: instru��o equal*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ( reg[a] == (s2) imm16 );
                  GLOBAL_n_relat++;
                  break;
      case 0x28:  /* [cmpgeui]: instru��o gran equal unsigned*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ( (u4) reg[a] >= (u4) imm16 );
                  GLOBAL_n_relat++;
                  break;
      case 0x30:  /* [cmpltui]: instru��o less unsigned*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ( (u4) reg[a] < (u4) imm16 );
                  GLOBAL_n_relat++;
                  break;
      
      /* instru��es l�gicas de opera��o bit a bit*/
      case 0x0C:  /* [andi]: instru��o andi (bit a bit)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] & (s2) imm16;
                  GLOBAL_n_logic++;
                  break;
      case 0x14:  /* [ori]: instru��o ori (bit a bit)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] | (s2) imm16;
                  GLOBAL_n_logic++;
                  break;
      case 0x1C:  /* [xori]: instru��o xori (bit a bit)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] ^ (s2) imm16;
                  GLOBAL_n_logic++;
                  break;
      case 0x2C:  /* [andhi]: instru��o andhi (bit a bit)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] & ( (u4) imm16 << 16 );
                  GLOBAL_n_logic++;
                  break;
      case 0x34:  /* [orhi]: instru��o orhi (bit a bit)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] | ( (u4) imm16 << 16 );
                  GLOBAL_n_logic++;
                  break;
      case 0x3C:  /* [xorhi]: instru��o xorhi (bit a bit)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] ^ ( (u4) imm16 << 16 );
                  GLOBAL_n_logic++;
                  break;
      
      /* instru��es aritm�ticas*/
      case 0x04:  /* [addi]: instru��o addi*/
                  if ( is_block(b) ) return 2;
                  reg[b] = reg[a] + (s2) imm16;
                  GLOBAL_n_add++;
                  break;
      case 0x24:  /* [muli]: instru��o muli (armazena os 32 bits de baixa ordem)*/
                  if ( is_block(b) ) return 2;
                  tmp_s8 = (s8) reg[a] * imm16;
                  reg[b] = tmp_s8;
                  GLOBAL_n_mul++;
                  break;
      
      /*instru��es de salto*/
      case 0x06:  /* [br]: unconditional branch*/
                  *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      case 0x0E:  /* [bge]: branch if gran equal*/
                  if ( reg[a] >= reg[b] )
                     *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      case 0x16:  /* [blt]: branch if less*/
                  if ( reg[a] < reg[b] )
                     *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      case 0x1E:  /* [bne]: branch not if equal*/
                  if ( reg[a] != reg[b] )
                     *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      case 0x26:  /* [beq]: branch if equal*/
                  if ( reg[a] == reg[b] )
                     *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      case 0x2E:  /* [bgeu]: branch if gran equal unsigned*/
                  if ( (u4) reg[a] >= (u4) reg[b] )
                     *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      case 0x36:  /* [bltu]: branch if less unsigned*/
                  if ( (u4) reg[a] < (u4) reg[b] )
                     *pc = ( *pc + 4 + imm16 ) - 4; /* pois pc ser� incrementado*/
                  GLOBAL_n_branch++;
                  break;
      
      /* carregamento de dados*/
      case 0x03:  /* [ldbu]: load byte from memory (unsigned)*/
      case 0x23:  /* [ldbuio]: load byte from i/o  (unsigned)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = (u4) ram[reg[a] + (s4) imm16];
                  GLOBAL_n_load++;
                  break;
      case 0x07:  /* [ldb]: load byte from memory*/
      case 0x27:  /* [ldbio]: load byte from i/o*/
                  if ( is_block(b) ) return 2;
                  reg[b] = (s4) ram[reg[a] + (s4) imm16];
                  GLOBAL_n_load++;
                  break;
      case 0x0B:  /* [ldhu]: load halfword from memory (unsigned)*/
      case 0x2B:  /* [ldhuio]: load halfword from i/o (unsigned)*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ram[reg[a] + (s4) imm16] << 8;
                  reg[b] = (u4) ( reg[b] | ram[reg[a] + (s4) ( imm16 + 1 )] );
                  GLOBAL_n_load++;
                  break;
      case 0x0F:  /* [ldh]: load halfword from memory*/
      case 0x2F:  /* [ldhio]: load halfword from i/o*/
                  if ( is_block(b) ) return 2;
                  reg[b] = ram[reg[a] + (s4) imm16] << 8;
                  reg[b] = (s4) ( reg[b] | ram[reg[a] + (s4) ( imm16 + 1 )] );
                  GLOBAL_n_load++;
                  break;
      case 0x17:  /* [ldw]: load word from memory*/
      case 0x37:  /* [ldwio]: load word from i/o*/
                  if ( is_block(b) ) return 2;
                  aux = reg[a];
                  /*printf("aux = %d, rA = %d, rB = %d, imm16 = %d.\n",aux,reg[a],reg[b],imm16);*/
                  reg[b] = ram[aux + (s4) imm16] << 24;
                  reg[b] = reg[b] | ( ram[aux + (s4) ( imm16 + 1 )] << 16 );
                  reg[b] = reg[b] | ( ram[aux + (s4) ( imm16 + 2 )] << 8 );
                  reg[b] = reg[b] | ( ram[aux + (s4) ( imm16 + 3 )] );
                  printf("\nLDW/LDWIO for address %u: value %d loaded.",imm16+reg[a],reg[b]);
                  GLOBAL_n_load++;
                  break;
      
      /* armazenamento de dados*/
      case 0x05:  /* [sdb]: store byte to memory*/
      case 0x25:  /* [sdbio]: store byte to i/o*/
                  ram[reg[a] + (s4) imm16] = reg[b];
                  GLOBAL_n_store++;
                  break;
      case 0x0D:  /* [sdh]: store halfword to memory*/
      case 0x2D:  /* [sdhio]: store halfword to i/o*/
                  ram[reg[a] + (s4) imm16] = reg[b] >> 8;
                  ram[reg[a] + (s4) ( imm16 + 1 )] = reg[b];
                  GLOBAL_n_store++;
                  break;
      case 0x15:  /* [sdw]: store word to memory*/
      case 0x35:  /* [sdwio]: store word to i/o*/
                  ram[reg[a] + (s4) imm16] = reg[b] >> 24;
                  ram[reg[a] + (s4) ( imm16 + 1 )] = reg[b] >> 16;
                  ram[reg[a] + (s4) ( imm16 + 2 )] = reg[b] >> 8;
                  ram[reg[a] + (s4) ( imm16 + 3 )] = reg[b];
                  printf("\nSTW/STWIO for address %u: value %d stored.",imm16+reg[a],reg[b]);
                  GLOBAL_n_store++;
                  break;
      
      /* outras instru��es*/
      case 0x1B:  /* [flushda]: para cache de endere�os*/
      case 0x33:  /* [initd]: inicializa��o de cache de dados*/
      case 0x3B:  /* [flushd]: para cache de dados*/
                  GLOBAL_n_others++;
                  break;
      
      /* n�o p�de decodificar a instru��o*/
      default:   return 3;
   }
   
   return 0;
}

/* interpreta��o de instru��o R-Type*/
char int_rtype ( u4 word, u4 *pc ){
   u4 opx, imm5;
   u4 a, b, c, j;
   u2 tmp_u2, i;
   s8 tmp_s8;
   u8 tmp_u8;
   
   opx = word << 15;
   opx = opx >> (5+6+15);
   
   imm5 = word << 21;         /*os 5 bits entre opx e op*/
   imm5 = imm5 >> (6+21);
   
   a = word >> 27;
   b = word << 5;
   b = b >> 27;
   c = word << 10;
   c = c >> 27;
   
   /*printf("\n---\nDEBUG:\n");
   printf("opx = %X\timm5 = %d\nrA = %u, rB = %u, rC = %u\n---\n",opx,imm5,a,b,c);*/
   
   /* decodifica��o da instru��o e dos operandos e execu��o da mesma*/
   switch ( opx ){
      /* instru��es l�gicas de compara��o*/
      case 0x08:  /* [cmpge]: instru��o gran equal*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] >= reg[b] ? 1 : 0;
                  GLOBAL_n_relat++;
                  break;
      case 0x10:  /* [cmplt]: instru��o less*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] < reg[b] ? 1 : 0;
                  GLOBAL_n_relat++;
                  break;
      case 0x18:  /* [cmpne]: instru��o not equal*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] != reg[b] ? 1 : 0;
                  GLOBAL_n_relat++;
                  break;
      case 0x20:  /* [cmpeq]: instru��o equal*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] == reg[b] ? 1 : 0;
                  GLOBAL_n_relat++;
                  break;
      case 0x28:  /* [cmpgeu]: instru��o gran equal unsigned*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] >= reg[b] ? 1 : 0;
                  GLOBAL_n_relat++;
                  break;
      case 0x30:  /* [cmpltu]: instru��o less unsigned*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] < reg[b] ? 1 : 0;
                  GLOBAL_n_relat++;
                  break;
      
      /* instru��es l�gicas de opera��o bit a bit*/
      case 0x06:  /* [nor]: instru��o nor (bit a bit)*/
                  if ( is_block(c) ) return 2;
                  reg[c] = ~( reg[a] | reg[b] );
                  GLOBAL_n_logic++;
                  break;
      case 0x0E:  /* [and]: instru��o and (bit a bit)*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] & reg[b];
                  GLOBAL_n_logic++;
                  break;
      case 0x16:  /* [or]: instru��o or (bit a bit)*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] | reg[b];
                  GLOBAL_n_logic++;
                  break;
      case 0x1E:  /* [xor]: instru��o xor (bit a bit)*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] ^ reg[b];
                  GLOBAL_n_logic++;
                  break;
      
      /* instru��es aritm�ticas*/
      case 0x07:  /* [mulxuu]: instru��o mulxuu -> rA e rB unsigned*/
                  if ( is_block(c) ) return 2;
                  tmp_u8 = (u8) reg[a] * reg[b];
                  reg[c] = tmp_u8 >> 32;
                  GLOBAL_n_mul++;
                  break;
      case 0x17:  /* [mulxsu]: instru��o mulxsu -> rB unsigned*/
                  if ( is_block(c) ) return 2;
                  tmp_s8 = (s8) reg[a] * ( (u8) reg[b] );
                  reg[c] = tmp_s8 >> 32;
                  GLOBAL_n_mul++;
                  break;
      case 0x1F:  /* [mulxss]: instru��o mulxss (armazena os 32 bits de alta ordem)*/
                  if ( is_block(c) ) return 2;
                  tmp_s8 = (s8) reg[a] * reg[b];
                  reg[c] = tmp_s8 >> 32;
                  GLOBAL_n_mul++;
                  break;
      case 0x24:  /* [divu]: instru��o div unsigned*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[b] != 0 ? reg[a] / reg[b] : 0;
                  GLOBAL_n_div++;
                  break;
      case 0x25:  /* [div]: instru��o div*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[b] != 0 ? reg[a] / reg[b] : 0;
                  GLOBAL_n_div++;
                  break;
      case 0x27:  /* [mul]: instru��o mul (armazena os 32 bits de baixa ordem)*/
                  if ( is_block(c) ) return 2;
                  tmp_s8 = (s8) reg[a] * reg[b];
                  reg[c] = tmp_s8;
                  GLOBAL_n_mul++;
                  break;
      case 0x31:  /* [add]: instru��o add*/
                  /*considerando a exist�ncia da instru��o NOP (add r0, r0, r0)*/
                  if ( is_block(c) && ( a != 0 || b != 0 || c != 0 ) ) return 2;
                  reg[c] = reg[a] + reg[b];
                  GLOBAL_n_add++;
                  break;
      case 0x39:  /* [sub]: instru��o sub*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] - reg[b];
                  GLOBAL_n_sub++;
                  break;
      
      /* instru��es de interrup��es e desvios*/
      case 0x01:  /* [eret]: retorno do exception handler*/
                  ctrl[status] = ctrl[estatus];
                  *pc = reg[ea] - 4;   /* pois vai incrementar!*/
                  GLOBAL_n_others++;
                  break;
      case 0x05:  /* [ret]: retorno de rotina*/
                  *pc = ctrl[ra] - 4;  /* pois vai incrementar!*/
                  GLOBAL_n_others++;
                  break;
      case 0x09:  /* [bret]: retorno de breakpoint*/
                  ctrl[status] = ctrl[bstatus];
                  *pc = reg[ba] - 4;   /* pois vai incrementar!*/
                  GLOBAL_n_others++;
                  break;
      case 0x0D:  /* [jmp]: um salto*/
                  *pc = reg[a] - 4;    /* pois vai incrementar!*/
                  GLOBAL_n_branch++;
                  break;
      case 0x1C:  /* [nextpc]: pr�xima instru��o*/
                  reg[c] = *pc + 4;
                  GLOBAL_n_others++;
                  break;
      case 0x1D:  /* [callr]: chamada de rotina*/
                  reg[ra] = *pc + 4;
                  *pc = reg[a] - 4;    /* pois vai incrementar!*/
                  GLOBAL_n_others++;
                  break;
      case 0x2D:  /* [trap]: chamada do tratador de exce��es*/
                  ctrl[estatus] = ctrl[status];
                  ctrl[status] = ( ctrl[status] >> 1 ) << 1;   /* zera PIE_bit*/
                  reg[ea] = *pc + 4;
                  /* aqui, ir para exception handler*/
                  GLOBAL_n_others++;
                  break;
      case 0x34:  /* [break]: chamada do processador de debug*/
                  ctrl[bstatus] = ctrl[status];
                  ctrl[status] = ( ctrl[status] >> 1 ) << 1;   /* zera PIE_bit*/
                  reg[ba] = *pc + 4;
                  /* aqui, ir para debug breaking processor*/
                  GLOBAL_n_others++;
                  break;
      
      /* instru��es de rota��o*/
      case 0x02:  /* [roli]: rota��o para a esquerda com valor imediato*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a];
                  for ( i = 0; i < imm5; i++ ){
                     j = reg[c] >> 31;   /* pega �ltimo bit*/
                     reg[c] = reg[c] << 1;
                     reg[c] |= j;
                  }
                  GLOBAL_n_rotat++;
                  break;
      case 0x03:  /* [rol]: rota��o para a esquerda*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a];
                  tmp_u2 = reg[b] << 27;
                  tmp_u2 = tmp_u2 >> 27;
                  for ( i = 0; i < tmp_u2; i++ ){
                     j = reg[c] >> 31;   /* pega �ltimo bit*/
                     reg[c] = reg[c] << 1;
                     reg[c] |= j;
                  }
                  GLOBAL_n_rotat++;
                  break;
      case 0x0B:  /* [ror]: rota��o para a direita*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a];
                  tmp_u2 = reg[b] << 27;
                  tmp_u2 = tmp_u2 >> 27;
                  for ( i = 0; i < tmp_u2; i++ ){
                     j = ( reg[c] << 31 ) >> 31;   /* pega o primeiro bit*/
                     reg[c] = reg[c] >> 1;
                     j << 31;
                     reg[c] |= j;
                  }
                  GLOBAL_n_rotat++;
                  break;
      case 0x12:  /* [slli]: shift para a esquerda com valor imediato*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] << imm5;
                  GLOBAL_n_rotat++;
                  break;
      case 0x13:  /* [sll]: shift para a esquerda*/
                  if ( is_block(c) ) return 2;
                  tmp_u2 = ( reg[b] << 27 ) >> 27;
                  reg[c] = reg[a] << tmp_u2;
                  GLOBAL_n_rotat++;
                  break;
      case 0x1A:  /* [srli]: shift para a direita unsigned com valor imediato*/
                  if ( is_block(c) ) return 2;
                  reg[c] = (u4) reg[a] >> imm5;
                  GLOBAL_n_rotat++;
                  break;
      case 0x1B:  /* [srl]: shift para a direita unsigned*/
                  if ( is_block(c) ) return 2;
                  tmp_u2 = ( reg[b] << 27 ) >> 27;
                  reg[c] = (u4) reg[a] >> tmp_u2;
                  GLOBAL_n_rotat++;
                  break;
      case 0x3A:  /* [srai]: shift para a direita com valor imediato*/
                  if ( is_block(c) ) return 2;
                  reg[c] = reg[a] >> imm5;
                  GLOBAL_n_rotat++;
                  break;
      case 0x3B:  /* [sra]: shift para a direita*/
                  if ( is_block(c) ) return 2;
                  tmp_u2 = ( reg[b] << 27 ) >> 27;
                  reg[c] = reg[a] >> tmp_u2;
                  GLOBAL_n_rotat++;
                  break;
      
      /* outras instru��es*/
      case 0x04:  /* [flushp]: para pipeline apenas*/
      case 0x0C:  /* [flushi]: para cache de instru��es*/
                  GLOBAL_n_others++;
                  break;
      case 0x26:  /* [rdctl]: ler de reg. de controle*/
                  if ( is_block(c) ) return 2;
                  reg[c] = ctrl[imm5];
                  GLOBAL_n_others++;
                  break;
      case 0x29:  /* [initi]: para cache de instru��es*/
                  GLOBAL_n_others++;
                  break;
      case 0x2E:  /* [wrctl]: escrita em reg. de controle*/
                  ctrl[imm5] = reg[a];
                  GLOBAL_n_others++;
                  break;
      case 0x36:  /* [sync]: para sincroniza��o de mem�ria*/
                  GLOBAL_n_others++;
                  break;
      
      /* nenhuma instru��o foi decodificada*/
      default:   return 3;
   }
   
   return 0;
}

/* interpreta��o de instru��o J-Type*/
char int_jtype ( u4 word, u4 *pc ){
   u4 op, imm26;
   
   op = word << 26;
   op = op >> 26;
   
   imm26 = word >> 6;
   
   /* [call]: �nica instru��o J-Type*/
   if ( op == 0x00 ){
      reg[ra] = *pc + 4;
      *pc = ( (*pc >> 28) << 28 ) | ( imm26 * 4 );
      GLOBAL_n_others++;
   }else return 3;
   
   return 0;
}
