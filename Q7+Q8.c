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
#include <errno.h>   /* errno */
#include <signal.h>   /* traitement des signaux */
#include "job.c" //Contient tout ce qui est relatif au stockage des processus, leur informations ainsi que leur affichage


/** Variables Globales. */    
pid_t pid ;// Le process fils.


/*handler de SIGTSTP*/
void handler_SIGTSTP(int sig) {
    set_etat(pid);
    kill(pid, SIGSTOP);

}
/*handler de SIGINT */
void handler_SIGINT(int sig) {
   kill(pid, SIGINT );
   delete_job(pid);
}

/*handler de SIGCHDL */
void handler_SIGCHLD (int sig) {

    int etat_fils, pid_fils;

    do {
        pid_fils = (int) waitpid(-1, &etat_fils, WNOHANG | WUNTRACED | WCONTINUED);
        if ((pid_fils == -1) && (errno != ECHILD)) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        } else if (pid_fils > 0) {
            int i = get_job_indice(pid_fils);
            if (i!= -1) {
                 if (WIFEXITED(etat_fils)) {
                    /* traiter exit */
                    delete_job(pid_fils);
                }
            }
        }

    } while (pid_fils > 0);
    return ;
}

int main(int argc, char *argv[]) {

   struct cmdline *cmdl; // la structure qui contient la ligne de commanade    
    /* Traiter les signaux SIGCHLD, SIGTSTP et SIGINT*/
    signal(SIGCHLD, handler_SIGCHLD);//Attribuer la handler au SIGCHLD
            signal(SIGTSTP,handler_SIGTSTP); //for Ctrl+Z
            signal(SIGINT, &handler_SIGINT); //for Ctrl+C
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
            
            // La commande "sj id" .
            else if (!strcmp(cmdl->seq[0][0], "sj")) {
                stop_processus(cmdl->seq[0]);
            }
        
            // La commande "bg id" .
            else if (!strcmp(cmdl->seq[0][0], "bg")) {
                background(cmdl->seq[0]);
            }
            // La commande "fg id" .
            else if (!strcmp(cmdl->seq[0][0], "fg")) {
                foreground(cmdl->seq[0]);
            }
        else{
        pid = fork();
        
        /* Lire la commande. */
        
        if (pid < 0) {   /* échec du fork */
                   perror("Erreur fork");
                }
 
         /* fils */
         if (pid == 0) {
             signal(SIGCHLD, handler_SIGCHLD);//Attribuer la handler au SIGCHLD
            signal(SIGTSTP,handler_SIGTSTP); //for Ctrl+Z
            signal(SIGINT, &handler_SIGINT); //for Ctrl+C
		       int exe = execvp(cmdl->seq[0][0],cmdl->seq[0]);
		      /* Vérifier si la commande est valide ou non. */
		      if(exe < 0){
			     perror("execvp");
                        }       
               }
	     else
            if (cmdl->backgrounded == NULL){
                //FG
                        /* Enregistrer le processus fils dans liste_jobs. */
                        add_job(pid, *cmdl->seq[0], fg);
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
