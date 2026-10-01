/*
Criação de processos em UNIX, com execução de outro binário

Compilar com gcc -Wall fork-execve.c -o fork-execve

Carlos Maziero, DINF/UFPR 2020

---------------------------------------------------------------------------
Exercício 2 - comentários
---------------------------------------------------------------------------
Combina fork() e execve(), que é a forma padrão de lançar um novo programa
em UNIX (é o que o shell faz a cada comando):
  1. fork() cria um processo filho, cópia do pai;
  2. o filho chama execve(), que SUBSTITUI a imagem do processo (código,
     dados, pilha, heap) pelo conteúdo do executável indicado (/bin/date).
     O PID continua o mesmo, mas o programa passa a ser outro;
  3. o pai espera o filho terminar com wait().
Se execve() tiver sucesso, ela NUNCA retorna: as instruções seguintes do
programa original deixam de existir no filho. Ela só retorna (com -1) se
falhar - por exemplo, se o arquivo não existir (errno = ENOENT).
*/

#include <unistd.h>     // fork(), execve(), getpid(), getppid()
#include <stdio.h>      // printf(), perror()
#include <stdlib.h>     // exit()
#include <sys/types.h>  // tipo pid_t
#include <sys/wait.h>   // wait()

// argv: argumentos da linha de comando deste programa
// envp: variáveis de ambiente (PATH, HOME, LANG, ...) - terceiro parâmetro
//       de main(), suportado em sistemas UNIX
int main (int argc, char *argv[], char *envp[])
{
  int retval ;  // valor de retorno de fork()

  // Executado só pelo processo original.
  printf ("Ola, sou o processo %5d\n", getpid()) ;

  // Cria o filho; a partir daqui há dois processos.
  retval = fork () ;

  // Executado pelo pai (retval = PID do filho) e pelo filho (retval = 0).
  printf ("[retval: %5d] sou %5d, filho de %5d\n", retval, getpid(), getppid()) ;

  if ( retval < 0 )       // erro no fork ()
  {
    perror ("Erro: ") ;   // (imprime "Erro: : <mensagem>", pois perror já
                          //  acrescenta ": " após o prefixo)
    exit (1) ;
  }
  else
    if ( retval > 0 )     // sou o processo pai
      wait (0) ;          // espera o filho (que estará executando "date")
                          // terminar
    else                  // sou o processo filho
    {
      // Substitui o programa do filho por /bin/date. Os argumentos e o
      // ambiente repassados são os do próprio programa: argv[0] vira o
      // "nome" do date (ignorado por ele) e argumentos extras passados
      // a ./fork-execve seriam interpretados pelo date (ex.: +%H:%M).
      execve ("/bin/date", argv, envp) ;

      // Só chega aqui se execve() FALHOU (ex.: arquivo inexistente ou
      // sem permissão de execução). Em caso de sucesso, esta linha não
      // existe mais no processo filho.
      perror ("Erro") ;   // ex.: "Erro: No such file or directory"
    }

  // Executado pelo pai, após o término do filho. O filho só executa esta
  // linha se o execve() tiver falhado.
  printf ("Tchau de %5d!\n", getpid()) ;
  exit (0) ;
}
