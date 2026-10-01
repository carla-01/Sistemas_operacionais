# Lab 2 – Criação de processos (respostas)

> Programas compilados e executados no MSYS2 (`gcc -Wall`), que emula `fork()` no Windows. Os PIDs mostrados são os da execução real; o PID 923 é o do shell bash.
> Os códigos comentados em detalhe estão em `fork.c`, `fork-execve.c` e `fork-print.c` (item 1 de cada exercício).
>
> **Legenda dos diagramas** (mesma convenção do enunciado): o tempo flui para baixo;
> `|` = processamento, `:` = espera, `*` = evento, `-+-` = início/fim do processo, `....>` = interação (criação / término).

---

## Exercício 1 – `fork.c`

### 1. Análise do código
- O processo original imprime `Ola, sou o processo 1091` (apenas uma vez — o fork ainda não ocorreu).
- `fork()` cria o filho, que é uma cópia do pai e começa a executar **na instrução seguinte ao fork**.
  O valor de retorno é o que diferencia os dois: PID do filho no pai, `0` no filho, `-1` em caso de erro.
- Os dois processos executam o segundo `printf`, em ordem não determinística (depende do escalonador).
- O pai chama `wait(0)` e fica bloqueado até o filho terminar; o filho dorme 5 s, imprime `Tchau` e termina com `exit(0)`.
- Quando o filho termina, `wait()` retorna no pai, que imprime seu `Tchau` e termina.

Saída real (MSYS2):
```
Ola, sou o processo  1091
[retval:  1092] sou  1091, filho de   923
[retval:     0] sou  1092, filho de  1091
Tchau de  1092!        <- após ~5 s
Tchau de  1091!
```
(As duas linhas `[retval...]` podem aparecer em qualquer ordem. Os dois `Tchau`, não: o pai só imprime depois do `wait`.)

> Observação: se a saída for redirecionada para arquivo (`./fork > saida.txt`), o `Ola` aparece **duas vezes**:
> o `stdout` passa a ser totalmente bufferizado, e o buffer ainda não esvaziado é copiado para o filho no `fork()`.
> Execução real com redirecionamento:
> ```
> Ola, sou o processo  1114
> [retval:     0] sou  1115, filho de  1114
> Tchau de  1115!
> Ola, sou o processo  1114
> [retval:  1115] sou  1114, filho de  1097
> Tchau de  1114!
> ```

### 2. Diagrama de tempo
```
                        P (pai, PID 1091)            F (filho, PID 1092)
                         start -+-
                                |
              printf("Ola...")  *
                        fork()  * ..........................>-+- start
   printf("[retval: 1092]...")  *                             *  printf("[retval:    0]...")
                       wait(0)  *                             *  sleep(5)
                                :                             :
                   (bloqueado)  :                             :  (suspenso por 5 s)
                                :                             :
                                :                             *  printf("Tchau de 1092!")
                                :        fim do filho         *  exit(0)
               wait(0) retorna  * <..........................-+- end
      printf("Tchau de 1091!")  *
                       exit(0)  |
                           end -+-
```
---

## Exercício 2 – `fork-execve.c`

### 1. Análise do código
- Igual ao exercício 1 até o `fork()` e o segundo `printf`.
- O filho chama `execve("/bin/date", argv, envp)`, que **substitui a imagem do processo** (código, dados, pilha)
  pelo programa `/bin/date`. O PID continua 1094, mas o código original deixa de existir nele.
- Como `execve` bem-sucedido **não retorna**, o filho nunca executa `perror` nem o `printf("Tchau...")`:
  quem imprime é o `date`, que depois termina normalmente.
- O pai espera com `wait(0)` e, após o término do filho (`date`), imprime `Tchau de 1093!`.
- `argv` e `envp` do programa são repassados ao `date` (argumentos extras, como `./fork-execve +%H:%M`, mudariam a saída do `date`).

Saída real (MSYS2):
```
Ola, sou o processo  1093
[retval:  1094] sou  1093, filho de   923
[retval:     0] sou  1094, filho de  1093
Thu Oct  1 12:55:00 -03 2026
Tchau de  1093!
```
Note que só existe **um** `Tchau` — o do pai.

### 2. Diagrama de tempo
```
                        P (pai, PID 1093)            F (filho, PID 1094)
                         start -+-
              printf("Ola...")  *
                        fork()  * ..........................>-+- start
   printf("[retval: 1094]...")  *                             *  printf("[retval:    0]...")
                       wait(0)  *                             *  execve("/bin/date", ...)
                                :                                  --- imagem substituida por /bin/date ---
                                :                             |  (executa o programa date,
                   (bloqueado)  :                             *    imprime data/hora)
                                :        fim do filho         *  exit do date
               wait(0) retorna  * <..........................-+- end
      printf("Tchau de 1093!")  *
                       exit(0)  |
                           end -+-
```
### 3. E se o programa a ser executado não existir?
`execve()` falha e **retorna -1**, com `errno = ENOENT`. A imagem do filho não é substituída, então ele continua
executando o código original: `perror("Erro")` imprime `Erro: No such file or directory`, depois o filho
executa o `printf("Tchau...")` e o `exit(0)` — ou seja, o filho passa a fazer o mesmo que o pai.
O `wait()` do pai retorna e ele também imprime seu `Tchau`.

Saída real (programa `fork-execve-falha.c`, que é o `fork-execve.c` com `/bin/date` trocado por `/bin/naoexiste`):
```
Ola, sou o processo  1172
[retval:  1173] sou  1172, filho de   923
[retval:     0] sou  1173, filho de  1172
Erro: No such file or directory
Tchau de  1173!
Tchau de  1172!
```

```
                        P (pai, PID 1172)            F (filho, PID 1173)
                         start -+-
              printf("Ola...")  *
                        fork()  * ..........................>-+- start
   printf("[retval: 1173]...")  *                             *  printf("[retval:    0]...")
                       wait(0)  *                             *  execve("/bin/naoexiste", ...) -> -1
                                :                             *  perror: "Erro: No such file or directory"
                   (bloqueado)  :                             *  printf("Tchau de 1173!")
                                :        fim do filho         *  exit(0)
               wait(0) retorna  * <..........................-+- end
      printf("Tchau de 1172!")  *
                       exit(0)  |
                           end -+-
```
---

## Exercício 3 – `fork-print.c`

### 1. Análise do código
- `x = 0` antes do `fork()`; o filho recebe uma **cópia** de `x` (espaços de endereçamento separados;
  no Linux, via *copy-on-write*).
- Os dois processos imprimem `x vale 0`.
- O pai faz `x = 0` (não muda nada) e espera o filho; o filho faz `x++` (**na sua cópia**: x = 1) e dorme 5 s.
- O filho imprime `x vale 1` e termina; o pai, após o `wait`, imprime `x vale 0`.
- Conclusão: a alteração feita pelo filho **não é visível** ao pai — processos não compartilham memória.
  Para trocar dados seria preciso IPC (pipes, memória compartilhada, etc.).

Saída real (MSYS2):
```
No processo  1095 x vale 0
No processo  1096 x vale 0
No processo  1096 x vale 1     <- após ~5 s
No processo  1095 x vale 0
```

### 2. Diagrama de tempo (com a evolução de `x`)
```
                        P (pai, PID 1095)            F (filho, PID 1096)
                         start -+-
                         x = 0  *
                        fork()  * ..........................>-+- start   [x = 0 (copia)]
   printf -> "x vale 0"  [x=0]  *                             *  printf -> "x vale 0"  [x=0]
   x = 0                 [x=0]  *                             *  x++                   [x=1]
                       wait(0)  *                             *  sleep(5)
                                :                             :
    (bloqueado)          [x=0]  :                             :  (suspenso 5 s)        [x=1]
                                :                             :
                                :                             *  printf -> "x vale 1"  [x=1]
                                :        fim do filho         *  exit(0)
               wait(0) retorna  * <..........................-+- end
   printf -> "x vale 0"  [x=0]  *
                       exit(0)  |
                           end -+-
```
Evolução de `x`:

| Momento                | `x` no pai | `x` no filho |
|------------------------|:----------:|:------------:|
| antes do `fork()`      | 0          | —            |
| logo após o `fork()`   | 0          | 0 (cópia)    |
| após `x = 0` / `x++`   | 0          | 1            |
| `printf` final         | 0          | 1            |
