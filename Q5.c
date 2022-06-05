/** (mini)Projet (mini)Shell
 ** Développer un interpréteur de commandes simplifié
 ** @author : Abdelmalek rhayoute
 */
#define YEL "\e[0;33m"
#define RESET "\e[0;27m"
#include <stdlib.h> /* EXIT_SUCCESS */
#include <stdio.h>  /* entrées sorties: printf ... */
#include <unistd.h> /* pimitives de base : fork, ...*/
#include <string.h> /* opérations sur les chaînes */
#include "readcmd.h"  /* lecture des commandes */
#include <sys/wait.h> /* wait */


int main(int argc, char *argv[]) {

   struct cmdline *cmdl; // la structure qui contient la ligne de commanade
    /* Boucle Infini qui décrit le comportement de base de l'interpréteur */
    while (1){
       printf( YEL "arhayout@Minishell:~$ " RESET);//indiquer que c'est un mini-shell
        fflush(stdout);//vider la mémoire tampon et forçer l'affichage
        cmdl = readcmd();
        /* La commande interne exit du minishell */
        if (!strcmp(cmdl->seq[0][0],"exit")) {
                   exit(0);
                }
         /* Traiter la commande cd sans lancer de processus fils. */
         else if (!strcmp(cmdl->seq[0][0],"cd")) {
                     if (cmdl->seq[0][1] == NULL) {
                           chdir("/");
                     } else {
                           chdir(cmdl->seq[0][1]);
                     }
              }
        else{
        pid_t pid = fork();
        pid_t cpid;
        
        /* Lire la commande. */
        
        if (pid < 0) {   /* échec du fork */
                   perror("fork");
                }
 
         /* fils */
         if (pid == 0) {
		       int exe = execvp(cmdl->seq[0][0],cmdl->seq[0]);
		      /* Vérifier si la commande est valide ou non. */
		      if(exe < 0){
			     perror("execvp");
                        }       
               }
	     else
             if (cmdl->backgrounded == NULL){
                cpid = wait(NULL); /* réapparition du parent */
                printf("Parent pid = %d\n", getpid());
                printf("Child pid = %d\n", cpid);
             }
	           
	     }
        }
	return EXIT_SUCCESS;
}
