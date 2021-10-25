# Solution d'une question d'examen en IFT 2001
## Ordonner des threads à l'aide de sémaphores
### Contenu
- main.c le programme complet
- CMakelists.txt pour les utilisateurs de CMake
- README le présent fichier

### Explications

Essentiellement le programme est contenu dans un seul fichier, main.c et la solution du problème est dans la dernière
fonction du programme: executerUnThread.  Il s'agissait seulement d'ajouter un sem_wait sur le thread courant, afin qu'il
bloque en attendant son tour, et un sem_post sur le numéro du thread suivant, afin qu'il sache que son tour est venu.

Une fonction initialisant le tableau ordre aléatoirement a été rajoutée, afin de permettre de tester et d'expérimenter à
loisir, et on peut redéfinir NTHRREADS à volonté (dans des limites raisonnables, évidemment).
