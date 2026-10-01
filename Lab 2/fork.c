/*
Criação de processos em UNIX.

Compilar com gcc -Wall fork.c -o fork

Carlos Maziero, DINF/UFPR 2020

---------------------------------------------------------------------------
Exercício 1 - comentários
---------------------------------------------------------------------------
O programa mostra a criação de um processo filho com fork(). A chamada
fork() cria uma cópia quase idêntica do processo que a invoca (mesmo
código, cópia dos dados, pilha, heap e descritores de arquivos abertos).
A partir do fork() existem DOIS processos executando o MESMO código, a
partir da instrução seguinte ao fork(). A única diferença visível é o
valor de retorno:
  - no processo pai, fork() retorna o PID do filho (valor > 0);
  - no processo filho, fork() retorna 0;
  - em caso de erro (nenhum filho criado), retorna -1 no pai.
O programa usa esse valor para decidir o que cada processo faz: o pai
espera o filho terminar (wait) e o filho "trabalha" por 5 segundos (sleep).
*/

#include <unistd.h>     // fork(), getpid(), getppid(), sleep()
#include <stdio.h>      // printf(), perror()
#include <stdlib.h>     // exit()
#include <sys/types.h>  // tipo pid_t
#include <sys/wait.h>   // wait()

int main ()
{
  int retval ;  // guardará o valor de retorno de fork()

  // Executado apenas pelo processo original (ainda não houve fork).
  // getpid() retorna o PID do processo corrente.
  printf ("Ola, sou o processo %5d\n", getpid()) ;

  // Cria o processo filho. A partir daqui há dois processos executando
  // este mesmo código, cada um com sua própria cópia de 'retval'.
  retval = fork () ;

  // Executado pelos DOIS processos (pai e filho), em ordem não
  // determinística (depende do escalonador):
  //  - no pai:   retval = PID do filho; getppid() = PID do shell;
  //  - no filho: retval = 0; getppid() = PID do pai.
  printf ("[retval: %5d] sou %5d, filho de %5d\n", retval, getpid(), getppid()) ;

  if ( retval < 0 )    // erro no fork(): o filho não foi criado
  {
    perror ("Erro") ;  // imprime a mensagem de erro associada a errno
    exit (1) ;         // encerra com código de erro
  }
  else
    if ( retval > 0 )  // sou o processo pai
      wait (0) ;       // bloqueia até que um filho termine (wait(NULL):
                       // não interessa o status de saída do filho)
    else               // sou o processo filho
      sleep (5) ;      // fica suspenso por 5 segundos

  // Executado pelos dois processos: primeiro pelo filho (ao acordar do
  // sleep), depois pelo pai (quando wait() retorna, após o fim do filho).
  printf ("Tchau de %5d!\n", getpid()) ;

  // Encerra o processo com status 0 (sucesso). No filho, isso faz com
  // que o wait() do pai retorne.
  exit (0) ;
}
