// PingPongOS - PingPong Operating System
// Implementação da fila genérica (lista circular duplamente encadeada) definida em queue.h.

#include <stdio.h>
#include "queue.h"

// Verifica se o elemento pertence à fila indicada.
// Retorno: 1 se pertence, 0 senão

static int queue_contains (queue_t *queue, queue_t *elem)
{
   queue_t *aux ;

   if (!queue || !elem)
      return 0 ;

   aux = queue ;
   do
   {
      if (aux == elem)
         return 1 ;
      aux = aux->next ;
   }
   while (aux != queue) ;

   return 0 ;
}

// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size (queue_t *queue)
{
   queue_t *aux ;
   int size = 0 ;

   if (!queue)
      return 0 ;

   aux = queue ;
   do
   {
      size++ ;
      aux = aux->next ;
   }
   while (aux != queue) ;

   return size ;
}

// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita pela função externa print_elem.

void queue_print (char *name, queue_t *queue, void print_elem (void*) )
{
   queue_t *aux ;

   printf ("%s: [", name) ;

   if (queue)
   {
      aux = queue ;
      do
      {
         if (aux != queue)
            printf (" ") ;
         if (print_elem)
            print_elem (aux) ;
         aux = aux->next ;
      }
      while (aux != queue) ;
   }

   printf ("]\n") ;
}


// Insere um elemento no final da fila.
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_append (queue_t **queue, queue_t *elem)
{
   queue_t *last ;

   if (!queue)
   {
      fprintf (stderr, "### queue_append: a fila nao existe\n") ;
      return -1 ;
   }

   if (!elem)
   {
      fprintf (stderr, "### queue_append: o elemento nao existe\n") ;
      return -2 ;
   }

   if (elem->prev || elem->next)
   {
      fprintf (stderr, "### queue_append: o elemento ja esta em uma fila\n") ;
      return -3 ;
   }

   // fila vazia: o elemento aponta para si mesmo
   if (!*queue)
   {
      elem->next = elem ;
      elem->prev = elem ;
      *queue = elem ;
      return 0 ;
   }

   // fila não vazia: insere entre o último e o primeiro
   last = (*queue)->prev ;
   elem->next = *queue ;
   elem->prev = last ;
   last->next = elem ;
   (*queue)->prev = elem ;

   return 0 ;
}
// Remove o elemento indicado da fila, sem o destruir.
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_remove (queue_t **queue, queue_t *elem)
{
   if (!queue)
   {
      fprintf (stderr, "### queue_remove: a fila nao existe\n") ;
      return -1 ;
   }

   if (!*queue)
   {
      fprintf (stderr, "### queue_remove: a fila esta vazia\n") ;
      return -2 ;
   }

   if (!elem)
   {
      fprintf (stderr, "### queue_remove: o elemento nao existe\n") ;
      return -3 ;
   }

   if (!queue_contains (*queue, elem))
   {
      fprintf (stderr, "### queue_remove: o elemento nao pertence a fila\n") ;
      return -4 ;
   }

   if (elem->next == elem)
   {
      // único elemento: a fila fica vazia
      *queue = NULL ;
   }
   else
   {
      elem->prev->next = elem->next ;
      elem->next->prev = elem->prev ;

      // se removeu o primeiro, o seguinte passa a ser o primeiro
      if (*queue == elem)
         *queue = elem->next ;
   }

   elem->next = NULL ;
   elem->prev = NULL ;

   return 0 ;
}
