# Lab 6 — Atividade 3

## Enunciado

Implemente sua versão de *rwlock* com **prioridade para
operações de escrita**. Isso significa que sempre que uma
operação de escrita for solicitada, novas operações de leitura
não poderão começar (mesmo que outras operações de leitura já
estejam acontecendo), até que a operação de escrita seja
atendida.

## Implementação de *rwlock*

A implementação de *rwlock* foi feita com os arquivos
`RWLock.h` e `RWLock.c`, de forma similar à *rwlock* da
biblioteca *pthread*.

### Tipos

| Tipo de `RWLock` | Tipo equivalente de *pthread* |
|------------------|-------------------------------|
| `RWLock`         | `pthread_rwlock_t`            |

### Funções

| Função de `RWLock` | Função equivalente de *pthread* |
|--------------------|---------------------------------|
| `InitializeRWLock` | `pthread_rwlock_init`           |
| `DestroyRWLock`    | `pthread_rwlock_destroy`        |
| `LockForRead`      | `pthread_rwlock_rdlock`         |
| `UnlockForRead`    | `pthread_rwlock_unlock`         |
| `LockForWrite`     | `pthread_rwlock_wrlock`         |
| `UnlockForRead`    | `pthread_rwlock_unlock`         |

## Análise

A biblioteca criada implementa o padrão de leitores e
escritores já visto em aula. A novidade é que os recursos
necessários (o lock, as variáveis contadoras e as variáveis
de condição) foram agrupadas em um tipo heterogêneo, que deve
ser inicializado e destruído para o uso do padrão. Fora isso, as
funções da bibliteca são usadas de forma similar às funções de
início/fim de leitura/escrita do padrão.

O membro `mutex` de `RWLock` é usado para realizar exclusão
mútua ao acessar os outros membros do objeto `RWLock` nas outras
funções da biblioteca.

A função `LockForRead` espera que o número de escritores do
objeto `RWLock` seja `0`, para só então incrementar o número de
leitores. Para isso, ele espera com a variável de condição
`readersQueue` do objeto `RWLock`. Nesse caso, ela recebe
sinalização para seguir da função `UnlockForWrite`.

A função `LockForWrite` espera que o número de escritores e
leitores do objeto `RWLock` seja `0`, para só então incrementar
o número de escritores. Para isso, ele espera com a variável de
condição `writersQueue` do objeto `RWLock`. Nesse caso, ela
recebe sinalização para seguir das funções `UnlockForRead` e
`UnlockForWrite`.

A função `UnlockForRead` decrementa o número de leitores do
objeto `RWLock`. Caso o número de leitores se torne `0`, ela
sinaliza para uma thread que espera na variável de condição
`writersQueue` do objeto `RWLock`.

A função `UnlockForWrite` decrementa o número de escritores do
objeto `RWLock`. Caso o número de escritores se torne `0`, ela
sinaliza para uma thread que espera na variável de condição
`writersQueue` e para todas as threads que esperam na variável
de condição `readersQueue` do objeto `RWLock`.

## Compilação e execução do código

Para gerar o binário do programa, basta executar:

```
gcc main.c list_int.c RWLock.c -o <CAMINHO-DO-BINÁRIO>
```

Para iniciar o programa, basta executar:

```
./<CAMINHO-DO-BINÁRIO> <NÚMER-DE-THREADS>
```
