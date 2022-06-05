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
#include <signal.h>   /* traitement des signaux */
#include "job.c" //Contient tout ce qui est relatif au stockage des processus, leur informations ainsi que leur affichage

int main(int argc, char *argv[]) {

   struct cmdline *cmdl; // la structure qui contient la ligne de commanade
    /* Boucle Infini qui décrit le comportement de base de l'interpréteur */
    while (1){
     printf( YEL "arhayout@Minishell:~$ " RESET);//indiquer que c'est un mini-shell
        fflush(stdout);//vider la mémoire tampon et forçer l'affichage
        cmdl = readcmd();
        
        
        
          // La commande interne exit du minishell 
        if (!strcmp(cmdl->seq[0][0],"exit")) {
                   exit(0);
		 
                }
         //Traiter la commande cd sans lancer de processus fils.
         else if (!strcmp(cmdl->seq[0][0],"cd")) {
                     if (cmdl->seq[0][1] == NULL) {
                           chdir("/");
                     } else {
                           chdir(cmdl->seq[0][1]);
                     }
             }
           // La commande "lj"
            else if (!strcmp(cmdl->seq[0][0], "lj")) {
                lister_jobs();
            }
            
            // La commande stop id .
            else if (!strcmp(cmdl->seq[0][0], "sj")) {
                stop_processus(cmdl->seq[0]);
            }
        
            // La commande bg id .
            else if (!strcmp(cmdl->seq[0][0], "bg")) {
                background(cmdl->seq[0]);
            }
            // La commande fg id .
            else if (!strcmp(cmdl->seq[0][0], "fg")) {
                foreground(cmdl->seq[0]);
            }
        else{
        pid_t pid = fork();
        
        /* Lire la commande. */
        
        if (pid < 0) {   /* échec du fork */
                   perror("Erreur fork");
                }
 
         /* fils */
         else if (pid == 0) {
		       int exe = execvp(cmdl->seq[0][0],cmdl->seq[0]);
		      /* Vérifier si la commande est valide ou non. */
		      if(exe < 0){
			     perror("execvp");
                        }       
               }
	     else{
             if (cmdl->backgrounded == NULL){
                //FG
                        /* Enregistrer le processus fils dans liste_jobs. */
                        add_job(pid, *cmdl->seq[0], fg);
                        pid_fg = pid;
                        wait(NULL);
       
                        
                    }
              else {
                        /* Enregistrer le processus fils dans liste_jobs. */
                        add_job(pid, *cmdl->seq[0], bg);
                        /*afficher l'ID et PID du processus en BG*/
                        printf("[%d]       %d   \n" ,liste_jobs.list[liste_jobs.N - 1].job_id ,liste_jobs.list[liste_jobs.N - 1].job_pid);
                    }
             }
         }      
	     
}
}