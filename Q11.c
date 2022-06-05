/** (mini)Projet (mini)Shell
 ** Développer un interpréteur de commandes simplifié
 ** @author : Abdelmalek rhayoute
 */
 #define YEL "\e[0;33m"
 #define RESET "\e[0;27m"
  #define RED "\e[0;31m"
#include <stdlib.h> /* EXIT_SUCCESS */
#include <stdio.h>  /* entrées sorties: printf ... */
#include <unistd.h> /* pimitives de base : fork, ...*/
#include <string.h> /* opérations sur les chaînes */
#include "readcmd.h"  /* lecture des commandes */
#include <sys/wait.h> /* wait */
#include <errno.h>   /* errno */
#include <fcntl.h>    /* Files operations */
#include <signal.h>   /* traitement des signaux */
#include "job.c" //Contient tout ce qui est relatif au stockage des processus, leur informations ainsi que leur affichage

/** Variables Globales. */    
pid_t pid ;// Le process fils.
int main(int argc, char *argv[]) {

   struct cmdline *cmdl; // la structure qui contient la ligne de commanade      
    /* Boucle Infini qui décrit le comportement de base de l'interpréteur */
    while (1){
         printf( YEL "arhayout@Minishell:~$ " RESET);//indiquer que c'est un mini-shell     
        fflush(stdout);//vider la mémoire tampon et forçer l'affichage
        /* Lire la commande. */
        cmdl = readcmd();   
                int fd[2];
        /*Q11. tupes complexes.*/
            int i=0;
            int fdd = 0;
            while (cmdl->seq[i]!= NULL) {
                 
                    pipe(fd);				/* Sharing bidiflow */
                    if ((pid = fork()) == -1) {
                        perror("fork");
                        exit(1);
                    }
                    else if (pid == 0) {
                        dup2(fdd, 0);
                        if (cmdl->seq[i+1]!= NULL) {
                            dup2(fd[1], 1);
                        }
                        close(fd[0]);
                        execvp((cmdl->seq)[i][0],(cmdl->seq)[i]);
                        exit(1);
                    }
                    else {
                        if (cmdl->backgrounded != NULL){
                            /* Collect childs */
                            int status;
                            waitpid(pid,&status,WUNTRACED); // réapparition du parent
                            /* Enregistrer le processus fils dans liste_jobs. */ 
                            add_job(pid, *cmdl->seq[i], bg);
                            }
                            else{
                            add_job(pid, *cmdl->seq[i], fg);
                            }
                        close(fd[1]);
                        fdd = fd[0];
                        
                        i++;
                    }
                }
        }        
}



