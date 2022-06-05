/** (mini)Projet (mini)Shell
 ** Développer un interpréteur de commandes simplifié
 ** @author : Abdelmalek rhayoute
 **version finale
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
#include <stdbool.h>

/** Variables Globales. */    
pid_t pid=0 ;// Le processus en cours              
bool ctrl_Z;
bool ctrl_c;
pid_t bg_pid=0 ;
bool bg_stop;
void handler_SIGCHLD(){
    int c_pid, wstatus ;
    do {
        c_pid = (int) waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED);
        if ((c_pid == -1) && (errno != ECHILD)) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        } else if (c_pid > 0){
            if WIFEXITED(wstatus) {
                if (get_job_indice( c_pid)!=-1){
                    
                    if(c_pid==bg_pid){bg_stop=true;bg_pid=0;}
                    delete_job(c_pid);
                    c_pid = 0;
                }
            } else if (WIFCONTINUED(wstatus)) {
                if ((get_job_indice( c_pid)!=-1) && (ctrl_Z == false)){
                    set_etat( c_pid, fg);
                }
            } else if (WIFSTOPPED(wstatus)) {
                if (ctrl_Z == false) {
                    
                    if (get_job_indice( c_pid)!=-1){
                    set_etat( c_pid,  suspendu);
                    if(c_pid==bg_pid){bg_stop=true;}                    }
                }
            }
        }
    } while(c_pid > 0);
}

void handler_SIGTSTP(){
    if (bg_pid != 0) {
        kill(bg_pid,SIGSTOP);
        printf("\n");
        printf("[%d]+ KILLED\n", bg_pid);
        set_etat( bg_pid,  suspendu);
        ctrl_Z = true;
        bg_stop=true;
    }
    fflush(stdin);
}

void handler_SIGINT() {
    if (bg_pid != 0) {
        kill(bg_pid,SIGKILL);
        printf("\n");
        printf("[%d]+ KILLED\n", bg_pid);
        delete_job(pid);
        ctrl_c = true;
        bg_stop=true;
    }
    fflush(stdin);
}





int main(int argc, char *argv[]) {

   struct cmdline *cmdl; // la structure qui contient la ligne de commanade    
   int descriptor_in, descriptor_out, dup2_descriptor; // descripteurs de fichiers  
    /* Boucle Infini qui décrit le comportement de base de l'interpréteur */
    while (1){
        ctrl_Z = false;
        ctrl_c = false;
        /* Traiter les signaux SIGCHLD, SIGTSTP et SIGINT*/
        signal(SIGCHLD, handler_SIGCHLD);//Attribuer la handler au SIGCHLD
        signal(SIGTSTP,handler_SIGTSTP); //for Ctrl+Z
        signal(SIGINT, handler_SIGINT); //for Ctrl+C

        printf( YEL "arhayout@Minishell:~$ " RESET);//indiquer que c'est un mini-shell     
        fflush(stdout);//vider la mémoire tampon et forçer l'affichage
        /* Lire la commande. */
        cmdl = readcmd();
        dup2_descriptor=0;

/*Q6-commandes internes du minishell  sans lancer de processus fils*/
        if (cmdl->seq[0]== NULL){continue;}
          // La commande exit 
        if (!strcmp(cmdl->seq[0][0],"exit")) {
                   exit(0);
		 
                }
         //Traiter la commande cd .
        else if (!strcmp(cmdl->seq[0][0],"cd")) {
                     if (cmdl->seq[0][1] == NULL) {
                           chdir("/");
                     } else {
                           chdir(cmdl->seq[0][1]);
                     }
             }
           // La commande lj
        else if (!strcmp(cmdl->seq[0][0], "lj")) {
                lister_jobs();
            }
            
            // La commande sj id .
        else if (!strcmp(cmdl->seq[0][0], "sj")) {
                stop_processus(cmdl->seq[0]);
            }
        
            // La commande bg id .
        else if (!strcmp(cmdl->seq[0][0], "bg")) {
                background(cmdl->seq[0]);
            }
            // La commande fg id .
        else if (!strcmp(cmdl->seq[0][0], "fg")) {
            int Id;
                if ((Id = foreground(cmdl->seq[0]))>0) {
                    bg_pid=Id;
                    bg_stop=false;
                    while(!bg_stop) {
                        pause();
                    }
                }
   
                
            }
        else{

/* Q9-S'il y a redirection de stdin ou stdout*/
            if (cmdl->in != NULL && cmdl->err == NULL) //S'il y a redirection de stdin vers un fichier
            {
                /*ouvrir le fichier des entrées */
                descriptor_in = open(cmdl->in, O_RDONLY);
                /* traiter systématiquement les retours d'erreur des appels */
                if (descriptor_in < 0)
                {
                      perror(cmdl->out);
                      exit(1);
                }
               /*redirection de l'entrée standard*/
                dup2_descriptor = dup2(descriptor_in, 0);
                if (dup2_descriptor == -1)
                {
                    /* échec du dup2 */
                    perror("Erreur dup2(in)\n");
                    exit(1);
                }
            }
            if (cmdl->out != NULL && cmdl->err == NULL){
                            /* ouvrir le fichier résultats */
                            descriptor_out = open(cmdl->out, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                            /* traiter systématiquement les retours d'erreur des appels */
                            if (descriptor_out < 0)
                            {
                                perror(cmdl->out);
                                exit(1);
                            }
                            /*redirection de la sortie standard*/
                            
                            if ((dup2_descriptor=dup2 (descriptor_out, 1)) == -1)
                            {
                                /* échec du dup2 */
                                perror("Erreur dup2(out)\n");
                                exit(1);
                            }
            }            
/*Q11. tubes complexes.*/
            int fd[2];
            int i=0;
            int fdd =0;
            while (cmdl->seq[i]!= NULL) {
                 
                    pipe(fd);				// Partage de bidiflow 
                    if ((pid = fork()) == -1) {
                        perror("fork");
                        exit(1);
                    }
                    else if (pid == 0) {

                        if (dup2(fdd, 0)== -1) { 
                                printf("Erreur dup2 (tubes)\n") ;
                                exit(1) ;
                                }
                        if (cmdl->seq[i+1]!= NULL) {
                            dup2(fd[1], 1);
                        }
                        execvp((cmdl->seq)[i][0],(cmdl->seq)[i]);
                        perror((cmdl->seq)[i][0]) ;
                        close(fd[0]);
                        exit(1);

                
                    }
                    else {
                        if (cmdl->backgrounded == NULL){
                            /* Enregistrer le processus fils dans liste_jobs. */ 
                            add_job(pid, *cmdl->seq[i], bg);
                            /* Collect childs */
                            //int status;
                            //waitpid(pid,&status,WUNTRACED); // réapparition du parent
                            bg_pid = pid;
                            bg_stop=false;
                            while(!bg_stop) {
                                pause();
                            }
                            
                            }
                            else{
                            add_job(pid, *cmdl->seq[i], bg);
                            }
                        close(fd[1]);
                        fdd = fd[0];
                    } i++;
                        }
                       

                    }
                    
               
                }
         
        
        }
         



