#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 5

// Structure permettant de passer les paramètres voulus à un thread: son numéro et le numéro du prochain en ligne

typedef struct {
    int numThread ;
    int prochain ;
} Parametres;

// L'ordre de passage des threads

int ordre[NTHREADS] ;

// Les sémaphores qui vont gérer le passage des threads

sem_t semaphore[NTHREADS] ;

// Fonctions auxiliaires

void initialiserOrdre() ;

void initialiserLesSemaphores() ;
void detruireLesSemaphores() ;

void demarrerLesThreads(pthread_t* t, void* callback, Parametres *p) ;
void synchroniserLesThreads(pthread_t* t) ;

void genererParametres(Parametres *p) ;
void *executerUnThread(void *vargp) ;

/***********************************************************************************************************************
 * PROGRAMME PRINCIPAL
 * @return 0
 */

int main() {

    // Tableau contenant les paramètres pour chaque thread
    Parametres parametres[NTHREADS] ;

    // Tableau des threads
    pthread_t threads[NTHREADS] ;

    initialiserOrdre() ;
    initialiserLesSemaphores() ;
    genererParametres(parametres) ;

    demarrerLesThreads(threads, executerUnThread, parametres) ;

    synchroniserLesThreads(threads) ;
    detruireLesSemaphores() ;
    return 0;
}

/***********************************************************************************************************************
 * On va initialiser l'ordre de passage des threads aléatoirement.  Ceci sert strictement à tester le reste du
 * programme avec des cas variés.  On pourrait simplement hardcoder un ordre quelconque.
 * L'ordre attendu de passage des threads sera ensuite affiché, afin de comparer avec l'output.
 */

void initialiserOrdre() {
    time_t t ;
    srand((unsigned) time(&t)) ;

    // ordre est initialisé comme séquence de 0 à NTHREADS
    for (int i = 0; i < NTHREADS; ++i) ordre[i] = i ;

    // On mêle l'ordre avec Ficher-Yates, qui est O(N)
    for (int i = 0; i < NTHREADS - 1; ++i) {
        int j = i + (rand() % (NTHREADS - i)) ;
        int tmp = ordre[i] ;
        ordre[i] = ordre[j] ;
        ordre[j] = tmp ;
    }
    printf("L'ordre de passage des threads a été calculé\n") ;
    int erdro[NTHREADS] ;
    for (int i = 0; i < NTHREADS; ++i) erdro[ordre[i]] = i ;
    for (int i = 0; i < NTHREADS; ++i) printf("Rang %d --> Thread numéro %d\n", i, erdro[i]) ;
}

/***********************************************************************************************************************
 * Tous les sémaphores sont initialisés à 0: le thread correspondant doit bloquer et attendre son tour, sauf UN: le
 * premier thread à passer est initialisé à 1 et aura donc la permission immédiate de s'exécuter.
 */

void initialiserLesSemaphores() {
    for (int i = 0; i < NTHREADS; ++i) {
        if (!ordre[i]) sem_init(&semaphore[i], 0, 1) ;
        else sem_init(&semaphore[i], 0, 0) ;
    }
}

/***********************************************************************************************************************
 * À la fin il faut détruire les sémaphores...
 */

void detruireLesSemaphores() {
    for (int i = 0; i < NTHREADS; ++i) sem_destroy(&semaphore[i]) ;
}

/***********************************************************************************************************************
 * Création et lancement des threads
 * @param t Adresse du tableau des threads
 * @param callback Fonction qu'exécute le thread
 * @param p Arguments passés au callback
 */

void demarrerLesThreads(pthread_t* t, void* callback, Parametres *p) {
    for (int i= 0; i < NTHREADS; ++i) {
        int erreur = pthread_create(&t[i], NULL, executerUnThread, (void *) &p[i]) ;
        if (erreur) {
            printf ("Erreur %d lancée lors de la création du thread numéro %d\n", erreur, i) ;
            exit(-1) ;
        } else printf ("Lancement du thread numéro %d\n", i) ;
    }
}

/***********************************************************************************************************************
 * On va attendre tous les threads créés avant de terminer le thread principal
 * @param t Adresse du tableau des threads
 */

void synchroniserLesThreads(pthread_t *t) {
    for (int i = 0; i < NTHREADS; ++i) pthread_join(t[i], NULL) ;
}

/***********************************************************************************************************************
 * Initialisation du tableau des paramètres: chaque thread va posséder son numéro propre, et le numéro du prochain
 * thread à exécuter.  Le tout est contenu dans un struct Parametres.
 * @param p Adresse du tableau des struct Parametres
 */

void genererParametres(Parametres* p) {
     int erdro[NTHREADS] ;
     for (int i = 0; i < NTHREADS; ++i) {
         erdro[ordre[i]] = i ;
     }

    for (int i = 0; i < NTHREADS; ++i) {
        p[i].numThread = i ;
        p[i].prochain = erdro[ (ordre[i] + 1) % NTHREADS] ;
    }
}

/***********************************************************************************************************************
 * Exécution du thread: le thread vérifie si on est rendu à son tour, avec sem_wait. Si non, il bloque en attendant
 * que le précédent lui fasse signe de procéder.  Si oui, il affiche ses informations à l'écran, et ensuite fait signe
 * au suivant de procéder à son tour.  NB: Le dernier signal ne servira à rien, il remet le sémaphore du premier thread
 * à 1 mais cet appel est perdu.
 * @param vargp Adresse du struct Parametres contenant les info du thread courant, casté en void*.
 * @return Rien du tout
 */

void* executerUnThread(void *vargp) {
    Parametres* p = (Parametres *) vargp ;

    sem_wait(&semaphore[p->numThread]) ;
    printf ("Thread %d, prochain thread %d \n", p->numThread, p->prochain) ;
    sem_post(&semaphore[p->prochain]) ;

    pthread_exit(0) ;
}
