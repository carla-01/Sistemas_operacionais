/*
Criação de processos em UNIX, com impressão de valores de variável.

Compilar com gcc -Wall fork-print.c -o fork-print

Carlos Maziero, DINF/UFPR 2020

---------------------------------------------------------------------------
Exercício 3 - comentários
---------------------------------------------------------------------------
Mostra que pai e filho têm ESPAÇOS DE ENDEREÇAMENTO SEPARADOS. Após o
fork(), o filho recebe uma CÓPIA das variáveis do pai (na prática o Linux
usa copy-on-write: as páginas só são duplicadas quando alguém escreve).
Assim, a variável x existe em duas cópias independentes: o x++ feito pelo
filho NÃO é visto pelo pai, e vice-versa. Processos não compartilham
memória por padrão; para trocar dados precisam de mecanismos de IPC
(pipes, memória compartilhada, sinais, etc.).
*/

#include <unistd.h>     // fork(), getpid(), sleep()
#include <stdio.h>      // printf(), perror()
#include <stdlib.h>     // exit()
#include <sys/types.h>  // tipo pid_t
#include <sys/wait.h>   // wait()

int main ()
{
  int retval, x ;

  x = 0 ;            // x = 0 no processo original

  // Cria o filho: ele recebe uma cópia de x (valendo 0) e de retval.
  retval = fork () ;

  // Executado pelos dois processos; ambos imprimem x = 0, cada um com
  // o seu PID.
  printf ("No processo %5d x vale %d\n", getpid(), x) ;

  if ( retval < 0 )  // erro no fork()
  {
    perror ("Erro") ;
    exit (1) ;
  }
  else
    if ( retval > 0 )  // processo pai
    {
      x = 0 ;          // altera APENAS a cópia do pai (continua 0)
      wait (0) ;       // espera o filho terminar
    }
    else               // processo filho
    {
      x++ ;            // altera APENAS a cópia do filho: x passa a 1
      sleep (5) ;      // espera 5 s - tempo de sobra para o pai "ver" a
                       // mudança, se a memória fosse compartilhada
    }

  // Filho imprime x = 1; depois o pai (após o wait) imprime x = 0,
  // provando que a alteração do filho não afetou o pai.
  printf ("No processo %5d x vale %d\n", getpid(), x) ;
  exit (0) ;
}
