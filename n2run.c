/* ========================================================================= */
/* Simulador para o processador Nios II da Altera
   Cria��o: 15-ago-2007
   Modificado: 01-mar-2009
   Criado por: Willian dos Santos Lima */
/* ========================================================================= */
#include <string.h>
#include "def_sim.h"
#include "int_sim.h"

/* ========================================================================= */
/* vari�veis globais*/
u4 GLOBAL_size;

u4 GLOBAL_n_add, GLOBAL_n_sub, GLOBAL_n_mul, GLOBAL_n_div;
u4 GLOBAL_n_logic, GLOBAL_n_relat, GLOBAL_n_rotat;
u4 GLOBAL_n_load, GLOBAL_n_store, GLOBAL_n_branch, GLOBAL_n_others;

/* ========================================================================= */
/* fun��o que retorna a palavra de mem�ria procurada*/
u4 get_word ( u4 pc ){
   u4 new_w;
   
   new_w = ram[pc] << 24;
   new_w = new_w | ( ram[pc+1] << 16 );
   new_w = new_w | ( ram[pc+2] << 8 );
   new_w = new_w | ram[pc+3];
   
   return new_w;
}

/* fun��o para realizar contagem das instru��es por categoria, bem como
   leitura das instru��es para a mem�ria simulada*/
char pre_process ( char *file_name ){
   FILE *prog;
   u1 byte;
   u4 i, pc, word, op, opx;
   
   if ( (prog = fopen( file_name, "rb" )) == NULL ){
      printf("ERROR: couldn't load program.\n");
      return 1;
   }
   
   /* determinando o tamanho do arquivo*/
   GLOBAL_size = 0;
   while ( fread( &byte, sizeof(u1), 1, prog ) == 1 ) GLOBAL_size++;
   
   /* efetuando a leitura de todo o arquivo para a mem�ria*/
   ram = malloc( ( GLOBAL_size + 1 ) * sizeof(u1) + 1024 * 1024 );   /* 1MB para dados*/
   rewind( prog );
   i = 0;
   while ( fread( &byte, sizeof(u1), 1, prog ) == 1 )
      ram[++i] = byte;
   
   /* inicializando contadores*/
   GLOBAL_n_add = GLOBAL_n_sub = GLOBAL_n_mul = GLOBAL_n_div = 0;
   GLOBAL_n_logic = GLOBAL_n_relat = GLOBAL_n_rotat = 0;
   GLOBAL_n_load = GLOBAL_n_store = GLOBAL_n_branch = GLOBAL_n_others = 0;
   
   /* fazendo contagem das instru��es (pseudo-instru��es n�o s�o consideradas)*/
   pc = 1;
   while ( pc <= GLOBAL_size ){
      word = get_word( pc );
      pc += 4;
      
      op = word << 26;
      op = op >> 26;
      
      if ( op == 0x3A ){
         /*R-type*/
         opx = word << 15;
         opx = opx >> (5+6+15);
         
         switch ( opx ){
            case 0x08:  /* [cmpge]: instru��o gran equal*/
            case 0x10:  /* [cmplt]: instru��o less*/
            case 0x18:  /* [cmpne]: instru��o not equal*/
            case 0x20:  /* [cmpeq]: instru��o equal*/
            case 0x28:  /* [cmpgeu]: instru��o gran equal unsigned*/
            case 0x30:  /* [cmpltu]: instru��o less unsigned*/
                        GLOBAL_n_relat++;
                        break;
            case 0x06:  /* [nor]: instru��o nor (bit a bit)*/
            case 0x0E:  /* [and]: instru��o and (bit a bit)*/
            case 0x16:  /* [or]: instru��o or (bit a bit)*/
            case 0x1E:  /* [xor]: instru��o xor (bit a bit)*/
                        GLOBAL_n_logic++;
                        break;
            case 0x31:  /* [add]: instru��o add*/
                        GLOBAL_n_add++;
                        break;
            case 0x39:  /* [sub]: instru��o sub*/
                        GLOBAL_n_sub++;
                        break;
            case 0x07:  /* [mulxuu]: instru��o mulxuu -> rA e rB unsigned*/
            case 0x17:  /* [mulxsu]: instru��o mulxsu -> rB unsigned*/
            case 0x1F:  /* [mulxss]: instru��o mulxss (armazena os 32 bits de alta ordem)*/
            case 0x27:  /* [mul]: instru��o mul (armazena os 32 bits de baixa ordem)*/
                        GLOBAL_n_mul++;
                        break;
            case 0x24:  /* [divu]: instru��o div unsigned*/
            case 0x25:  /* [div]: instru��o div*/
                        GLOBAL_n_div++;
                        break;
            case 0x0D:  /* [jmp]: um salto*/
                        GLOBAL_n_branch++;
                        break;
            case 0x02:  /* [roli]: rota��o para a esquerda com valor imediato*/
            case 0x03:  /* [rol]: rota��o para a esquerda*/
            case 0x0B:  /* [ror]: rota��o para a direita*/
            case 0x12:  /* [slli]: shift para a esquerda com valor imediato*/
            case 0x13:  /* [sll]: shift para a esquerda*/
            case 0x1A:  /* [srli]: shift para a direita unsigned com valor imediato*/
            case 0x1B:  /* [srl]: shift para a direita unsigned*/
            case 0x3A:  /* [srai]: shift para a direita com valor imediato*/
            case 0x3B:  /* [sra]: shift para a direita*/
                        GLOBAL_n_rotat++;
                        break;
            default:    GLOBAL_n_others++;
         }
      }else if ( op == 0x00 ){
         /*J-type*/
         GLOBAL_n_others++;
      }else{
         /*I-type*/
         switch ( op ){
            case 0x08:  /* [cmpgei]: instru��o gran equal*/
            case 0x10:  /* [cmplti]: instru��o less*/
            case 0x18:  /* [cmpnei]: instru��o not equal*/
            case 0x20:  /* [cmpeqi]: instru��o equal*/
            case 0x28:  /* [cmpgeui]: instru��o gran equal unsigned*/
            case 0x30:  /* [cmpltui]: instru��o less unsigned*/
                        GLOBAL_n_relat++;
                        break;
            case 0x0C:  /* [andi]: instru��o andi (bit a bit)*/
            case 0x14:  /* [ori]: instru��o ori (bit a bit)*/
            case 0x1C:  /* [xori]: instru��o xori (bit a bit)*/
            case 0x2C:  /* [andhi]: instru��o andhi (bit a bit)*/
            case 0x34:  /* [orhi]: instru��o orhi (bit a bit)*/
            case 0x3C:  /* [xorhi]: instru��o xorhi (bit a bit)*/
                        GLOBAL_n_logic++;
                        break;
            case 0x04:  /* [addi]: instru��o addi*/
                        GLOBAL_n_add++;
                        break;
            case 0x24:  /* [muli]: instru��o muli (armazena os 32 bits de baixa ordem)*/
                        GLOBAL_n_mul++;
                        break;
            case 0x06:  /* [br]: unconditional branch*/
            case 0x0E:  /* [bge]: branch if gran equal*/
            case 0x16:  /* [blt]: branch if less*/
            case 0x1E:  /* [bne]: branch not if equal*/
            case 0x26:  /* [beq]: branch if equal*/
            case 0x2E:  /* [bgeu]: branch if gran equal unsigned*/
            case 0x36:  /* [bltu]: branch if less unsigned*/
                        GLOBAL_n_branch++;
                        break;
            case 0x03:  /* [ldbu]: load byte from memory (unsigned)*/
            case 0x23:  /* [ldbuio]: load byte from i/o  (unsigned)*/
            case 0x07:  /* [ldb]: load byte from memory*/
            case 0x27:  /* [ldbio]: load byte from i/o*/
            case 0x0B:  /* [ldhu]: load halfword from memory (unsigned)*/
            case 0x2B:  /* [ldhuio]: load halfword from i/o (unsigned)*/
            case 0x0F:  /* [ldh]: load halfword from memory*/
            case 0x2F:  /* [ldhio]: load halfword from i/o*/
            case 0x17:  /* [ldw]: load word from memory*/
            case 0x37:  /* [ldwio]: load word from i/o*/
                        GLOBAL_n_load++;
                        break;
            case 0x05:  /* [sdb]: store byte to memory*/
            case 0x25:  /* [sdbio]: store byte to i/o*/
            case 0x0D:  /* [sdh]: store halfword to memory*/
            case 0x2D:  /* [sdhio]: store halfword to i/o*/
            case 0x15:  /* [sdw]: store word to memory*/
            case 0x35:  /* [sdwio]: store word to i/o*/
                        GLOBAL_n_store++;
                        break;
            default:    GLOBAL_n_others++;
         }
      }
   }
   return 0;
}

/* fun��o respons�vel pela interpreta��o de um programa para Nios II*/
char interpret ( void ){
   char err;
   u1 byte;
   u4 word, op;
   u4 i, pc;
   
   /* setando os registradores dispon�veis do processador*/
   block = 0xFF000003;   /* neste caso: 11111111 00000000 00000000 00000011*/
   
   /* setando para 0 todos os bits de todos os registradores*/
   memset( reg, 0x0, sizeof(reg) );
   memset( ctrl, 0x0, sizeof(ctrl) );
   
   /* zerando contadores para contagem de instru��es executadas*/
   GLOBAL_n_add = GLOBAL_n_sub = GLOBAL_n_mul = GLOBAL_n_div = 0;
   GLOBAL_n_logic = GLOBAL_n_relat = GLOBAL_n_rotat = 0;
   GLOBAL_n_load = GLOBAL_n_store = GLOBAL_n_branch = GLOBAL_n_others = 0;
   
   printf("Starting simulation (file size %u) ...\n\n",GLOBAL_size);
   /* leitura das palavras contendo as instru��es*/
   pc = 1; i = 0;
   while ( pc <= GLOBAL_size ){
      if ( pc + 3 > GLOBAL_size ) return 4;  /* estourou a RAM*/
      word = get_word( pc );  /* obt�m a palavra (PC - PC+3)*/
      
      printf("\npc = %u.",pc);
      
      /* determinar o tipo de instru��o: obtendo apenas os 6 primeiros bits*/
      op = word << 26;
      op = op >> 26;
      
      if ( op == 0x3A ) err = int_rtype( word, &pc );
      else if ( op == 0x00 ) err = int_jtype( word, &pc );
           else err = int_itype( word, &pc );
      
      if ( err == 2 ){
         printf("ERROR: access to private register.\n");
         return 2;
      }else if ( err == 3 ){
               printf("ERROR: couldn't decode instruction.\n");
               return 3;
            }
      
      /* incremento da instru��o (PC)*/
      pc += 4; i++;
   }
   
   /* exibi��o da estrutura de mem�ria simulada*/
   for ( i = GLOBAL_size; i < GLOBAL_size + 250 /*parametriz�vel*/; i += 4 )
      printf("\nram[%d] = %d.",i,ram[i]<<24 | ram[i+1]<<16 | ram[i+2]<<8 | ram[i+3]);
   putchar('\n');
   
   /* desalocando a RAM*/
   free( ram );
   
   return 0;
}

/* ========================================================================= */
/* programa principal*/
int main ( int argc, char *argv[] ){
   char err;
   u2 i;
   u4 ini, fim;
   
   if ( argc < 2 ){
      printf("Insuficient parameters.\n");
      exit(-1);
   }
   
   /* pr�-processamento do arquivo bin�rio*/
   pre_process ( argv[1] );
   
   /* estat�sticas sobre as instru��es do programa simulado*/
   printf("\n=============================\n");
   printf("Intructions in code by type:\n");
   printf("Add    : %u\n",GLOBAL_n_add);      /*adi��o*/
   printf("Sub    : %u\n",GLOBAL_n_sub);      /*subtra��o*/
   printf("Mul    : %u\n",GLOBAL_n_mul);      /*multiplica��o*/
   printf("Div    : %u\n",GLOBAL_n_div);      /*divis�o*/
   printf("Logic. : %u\n",GLOBAL_n_logic);    /*opera��o l�gica*/
   printf("Relat. : %u\n",GLOBAL_n_relat);    /*compara��o relacional*/
   printf("Rotat. : %u\n",GLOBAL_n_rotat);    /*rota��o de bits*/
   printf("Load   : %u\n",GLOBAL_n_load);     /*carregamento de dados*/
   printf("Store  : %u\n",GLOBAL_n_store);    /*armazenamento de dados*/
   printf("Branch : %u\n",GLOBAL_n_branch);   /*salto*/
   printf("Store  : %u\n",GLOBAL_n_others);   /*outras categorias*/
   
   /* in�cio da simula��o*/
   ini = clock();
   err = interpret( );
   fim = clock();
   
   if ( err ) printf("Segmentation fault!\n");
   else{
      printf("Success in running program.\nState of registers:\n");
      /* exibindo os registradores, ainda n�o definitivo*/
      for ( i = 0; i < 32; i++ )
          printf("reg[%02d] = %d\n",i,reg[i]);
   }
   
   /* estat�sticas sobre as instru��es do programa simulado*/
   printf("\n=============================\n");
   printf("Intructions executed by type:\n");
   printf("Add    : %u\n",GLOBAL_n_add);      /*adi��o*/
   printf("Sub    : %u\n",GLOBAL_n_sub);      /*subtra��o*/
   printf("Mul    : %u\n",GLOBAL_n_mul);      /*multiplica��o*/
   printf("Div    : %u\n",GLOBAL_n_div);      /*divis�o*/
   printf("Logic. : %u\n",GLOBAL_n_logic);    /*opera��o l�gica*/
   printf("Relat. : %u\n",GLOBAL_n_relat);    /*compara��o relacional*/
   printf("Rotat. : %u\n",GLOBAL_n_rotat);    /*rota��o de bits*/
   printf("Load   : %u\n",GLOBAL_n_load);     /*carregamento de dados*/
   printf("Store  : %u\n",GLOBAL_n_store);    /*armazenamento de dados*/
   printf("Branch : %u\n",GLOBAL_n_branch);   /*salto*/
   printf("Store  : %u\n",GLOBAL_n_others);   /*outras categorias*/
   
   printf("\nElapsed time: %.3lfms.\n",(double)(fim-ini)/CLOCKS_PER_SEC);
   
   return 0;
}
