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
   /* Descripteur de pipe.*/
    int fd[2] ;     /* Boucle Infini qui décrit le comportement de base de l'interpréteur */
    while (1){
         printf( YEL "arhayout@Minishell:~$ " RESET);//indiquer que c'est un mini-shell     
        fflush(stdout);//vider la mémoire tampon et forçer l'affichage
        /* Lire la commande. */
        cmdl = readcmd();   
        pid = fork();
        
        
        if (pid < 0) {   /* échec du fork */
                   perror("fork");
                }
         /* fils */
        else if (pid == 0) {
        /*Q10. tupes simples, exécution de deux commande.*/
            if (cmdl->seq[1]!= NULL) {
                        int retour,descriptor1,descriptor2;
                        retour = pipe(fd) ;
                        if (retour == -1) {
                            printf("Erreur pipe\n") ;
                            exit(1) ;
                        }
                        int child;
                        child = fork() ;
                        if (child < 0) {
                            printf("Erreur fork\n") ;
                            exit(1) ;
                        }
                        else if (child == 0) {
                            close(fd[0]);
                            /*dup2 function creates a copy of the given file descriptor and assigns a new integer to it*/
                            descriptor1=dup2(fd[1],1);
                            if(descriptor1==-1){
                            printf("Erreur dup2");
                            }
                            close(fd[1]);
                            execlp((cmdl->seq)[0][0],(cmdl->seq)[0][0],NULL) ;    
                        } 
                        else {
                                close(fd[1]) ;
                                descriptor2 = dup2(fd[0], 0) ;
                                if (descriptor2 == -1) { 
                                printf("Erreur dup2\n") ;
                                exit(1) ;
                                }
                                close(fd[0]);
                                execlp((cmdl->seq)[1][0],(cmdl->seq)[1][0], (cmdl->seq)[1][1], NULL) ;
                                perror((cmdl->seq)[1][0]) ;
                                exit(1) ;
                        }
            }
            else{
                printf( RED "Ce programe est un test de la question 10.\n");
            }
        }
        else{
        /*Pointer to an area where status information about how the child process ended is to be placed.*/
        int status;
        /**f status information is not available, waitpid returns a 0 . WUNTRACED. causes the call to waitpid
        **to return status information for a specified process that has either stopped or terminated. Normally,
        **status information is returned only for terminated processes.
        */
        waitpid(pid,&status,WUNTRACED);
        }
    }
    return EXIT_SUCCESS;
}        



