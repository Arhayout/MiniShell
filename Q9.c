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
#include <fcntl.h>    /* Files operations */
#include <signal.h>   /* traitement des signaux */
#include "job.c" //Contient tout ce qui est relatif au stockage des processus, leur informations ainsi que leur affichage


/** Variables Globales. */    
pid_t pid ;// Le process fils.


int main(int argc, char *argv[]) {

   struct cmdline *cmdl; // la structure qui contient la ligne de commanade    
    int descriptor_in, descriptor_out, dup2_descriptor; /* descripteurs de fichiers */
     /* Boucle Infini qui décrit le comportement de base de l'interpréteur */
    while (1){
         printf( YEL "arhayout@Minishell:~$ " RESET);//indiquer que c'est un mini-shell     
        fflush(stdout);//vider la mémoire tampon et forçer l'affichage
         /* Lire la commande. */
        cmdl = readcmd();
        pid = fork();

        if (pid < 0) {   /* échec du fork */
                   perror("fork");
                }
        
        else if (pid == 0) {

         //S'il y a redirection de stdin ou stdout

        if (cmdl->in != NULL && cmdl->err == NULL) //S'il y a redirection de stdin vers un fichier
                {
                    /*ouvrir le fichier des entrées */
                    descriptor_in = open(cmdl->in, O_RDONLY);
                    /* traiter systématiquement les retours d'erreur des appels */
                    if (descriptor_in < 0)
                    {
                        fprintf(stderr, "Erreur ouverture %s\n", cmdl->in);
                        exit(1);
                    }
                    //rediriger stdin vers le fichier de descriptor_in
                    dup2_descriptor = dup2(descriptor_in, 0);
                    if (dup2_descriptor == -1)
                    {
                        /* échec du dup2 */
                        perror("Erreur dup2\n");
                        exit(1);
                    }
                }
        if (cmdl->out != NULL && cmdl->err == NULL) //S'il y a rediréction de stdout vers un fichier
                {

                    /* ouvrir le fichier résultats */
                    descriptor_out = open(cmdl->out, O_WRONLY | O_CREAT | O_TRUNC, 0640);
                    /* traiter systématiquement les retours d'erreur des appels */
                    if (descriptor_out < 0)
                    {
                        fprintf(stderr, "Erreur ouverture %s\n", cmdl->out);
                        exit(2);
                    }
                    //rediriger stdout vers le fichier de descriptor_out
                    dup2_descriptor = dup2(descriptor_out, 1);
                    if (dup2_descriptor == -1)
                    {
                        /* échec du dup2 */
                        perror("Erreur dup2\n");
                        exit(5);
                    }
                }

                
 
                /* fils */
               
                int exe = execvp(cmdl->seq[0][0],cmdl->seq[0]);
		      /* Vérifier si la commande est valide ou non. */
		        if(exe < 0){
			     perror("execvp");
                        } 
            
	   } 
       else{

        int status;
        waitpid(pid,&status,WUNTRACED);

       }
    }  
            return EXIT_SUCCESS; 
}     


