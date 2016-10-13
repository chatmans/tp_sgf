#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<syr1_file.h>

int syr1_fopen_read(char *name, SYR1_FILE* file);
int syr1_fopen_write(char *name, SYR1_FILE *file);
int syr1_fclose_read(SYR1_FILE* file);
int syr1_fclose_write(SYR1_FILE *file);

/* SYNOPSYS :
 * 	  SYR1_FILE*  syr1_fopen(char *name, char *mode);
 * DESCRIPTION :
 *   gère l'ouverture d'un fichier logique.
 * PARAMETRES :
 *   name : chaîne de caractères contenant le nom externe du fichier à ouvrir
 *   mode : chaîne de caractères spécifiant le mode d'ouverture ("r" ou "w")
 * RESULTAT :
 *   La fonction retourne un pointeur sur un Bloc Contrôle Fichier en
 *   cas d'ouverture réussie et NULL en cas d'échec (un message d'erreur est
 *   affiché pour indiquer l'origine de l'erreur).
 */
SYR1_FILE * syr1_fopen(char *name, char *mode)
{
  int erreur = 0;

  SYR1_FILE  *file = alloc_logical_file(name,mode);

  if (file != NULL) {
    if (strcmp(mode,"r")==0) {
      erreur = syr1_fopen_read(name,file);
    }
    else if (strcmp(mode,"w")==0) {
      erreur = syr1_fopen_write(name,file);
    }
    else {
      erreur =-4;
    }
  }
  if (erreur<0) {
    free_logical_file(file);
    return (SYR1_FILE*) NULL;
  }
  else {
    return file;
  }
}

/* SYNOPSYS :
 * 	  int syr1_fclose(SYR1_FILE* file)
 * DESCRIPTION :
 *   gère la fermeture d'un fichier logique.
 * PARAMETRES :
 *   file : pointeur sur un Bloc de Contrôle Fichier (BCF)
 * RESULTAT :
 *    0 : la fermeture a réussi
 *   -1 : problème pendant la libération du descripteur de fichier logique
 *        (ou le fichier logiques file vaut NULL)
 *   -2 : erreur d'entrée-sortie sur le périphérique de stockage
 */
int syr1_fclose(SYR1_FILE* file)
{
  if (file == NULL) {
    return -1;
  }
  else if (strcmp(file->mode,"w")==0) {
    return syr1_fclose_write(file);
  }
  else if (strcmp(file->mode,"r")==0) {
    return syr1_fclose_read(file);
  }
  else {
    return -1;
  }
}

