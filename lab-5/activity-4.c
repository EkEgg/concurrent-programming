/**
 * Activity 4:
 * 
 * 1. Revisit the soma-lock-atom.c program from Lab 4.
 * 
 * 2. Alter it so the extra thread prints the first 20 values of soma which are multiples of 10.
 *    The goal is to make the ExecutaTarefa thread pause its execution when a multiple of 10 is
 *    reached and only unpause after its value is printed.
 * 
 * 3. Run the program and confirm all requirements were met.
 * 
 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0;
long int lastPrinted;
int finishedPrinting = 0;
int waitForPrint = 0;
pthread_mutex_t mutex;
pthread_cond_t waitingFor10;
pthread_cond_t waitingForPrint;

void* ExecutaTarefa(void* arg)
{
    long int id = (long int) arg;
    printf("Thread : %ld esta executando...\n", id);

    for (int i = 0; i < 100000; i++)
    {
        pthread_mutex_lock(&mutex);
        while (waitForPrint)
        {
            pthread_cond_wait(&waitingForPrint, &mutex);
        }
        soma++;
        if (!finishedPrinting && soma % 10 == 0)
        {
            waitForPrint = 1;
            pthread_cond_signal(&waitingFor10);
            pthread_cond_wait(&waitingForPrint, &mutex);
            waitForPrint = 0;
            pthread_cond_broadcast(&waitingForPrint);
        }
        pthread_mutex_unlock(&mutex);
    }

    printf("Thread : %ld terminou!\n", id);
    pthread_exit(NULL);
}

void* extra(void* args)
{
    printf("Extra : esta executando...\n");

    for (int i = 0; i < 20; i++)
    {
        pthread_mutex_lock(&mutex);
        if (soma == lastPrinted || soma % 10 != 0)
        {
            pthread_cond_wait(&waitingFor10, &mutex);
        }
        printf("soma = %ld \n", soma);
        lastPrinted = soma;
        if (i+1 >= 20)
        {
            finishedPrinting = 1;
        }
        pthread_cond_broadcast(&waitingForPrint);
        pthread_mutex_unlock(&mutex);
    }

    printf("Extra : terminou!\n");
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    pthread_t *tid;
    int nthreads;

    if (argc < 2)
    {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    tid = (pthread_t*) malloc((sizeof (pthread_t)) * (nthreads+1));
    if (tid == NULL)
    {
        puts("ERRO--malloc");
        return 2;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&waitingFor10, NULL);
    pthread_cond_init(&waitingForPrint, NULL);

    for (long int t = 0; t < nthreads; t++)
    {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void*) t))
        {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    if (pthread_create(&tid[nthreads], NULL, extra, NULL))
    {
        printf("--ERRO: pthread_create()\n"); exit(-1);
    }

    for (int t = 0; t < nthreads+1; t++)
    {
        if (pthread_join(tid[t], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1); 
        } 
    } 

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&waitingFor10);
    pthread_cond_destroy(&waitingForPrint);

    printf("Valor de 'soma' = %ld\n", soma);

    return 0;
}
