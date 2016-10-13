#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<syr1_file.h>
#include<physical_io.h>

/* SYNOPSYS :
 * 	  int syr1_fopen_read(char *name, SYR1_FILE *file) {
 * DESCRIPTION :
 *   Ce sous-programme gère l'ouverture d'un fichier logique en mode lecture.
 * PARAMETRES :
 *   name : chaîne de caratère contenant le nom externe du fichier à ouvrir
 *   file : pointeur sur un Bloc Control Fichier (File Control Bloc)
 * RESULTAT :
 *    0 : ouverture réussie
 *   -1 : autre erreur
 */
int syr1_fopen_read(char *name, SYR1_FILE* file) {
  int search = search_entry(name, file->descriptor); 
  switch(search){
    case 0: unsigned char* buffer = malloc (IO_BLOCK_SIZE);
      if (buffer == NULL) return -1;
      file->buffer = buffer;
      file->mode = 'r';
      file->current_block = 0;
      file->file_offset = 0;
      file->block_offset = 0;
      return 0;
    case default: 
      return -1;
  }
}

/*
 * SYNOPSYS :
 * 	 int syr1_fread(SYR1_FILE *file, int item_size, int nbitem, char* buf)
 * DESCRIPTION :
 *   Ce sous-programme lit nbitem articles de taille item_size dans le fichier
 *   fichier logique passé en paramètre.
 * PARAMETRES :
 *   	 file : pointeur sur un Bloc Control Fichier (File Control Bloc)
 *  item_size : taille d'un article
 *    nb_item : nombre d'article à lire
 * RESULTAT :
 *   le nombre d'articles effectivement lus dans le fichier, sinon un code
 *   d'erreur (cf syr1_getc())
 *    -1 : le BCF est NULL, ou le mode d'ouverture est incorrect
 *    -2 : erreur d'entrée-sorties sur le périphérique de stockage
 *    -3 : fin de fichier
 */
int syr1_fread(SYR1_FILE *file, int item_size, int nbitem, char* buf) {
  int count = 0;
  while (count<nbitem*item_size) {
    int res = syr1_getc(file);
    if (res<0) {
      return res;
    } else {
      buf[count]=(unsigned char) res;
    }
    count++;
  }
  return count/item_size;
}


/*
 * SYNOPSYS :
 * 	 int syr1_getc(SYR1_FILE *file)
 * DESCRIPTION :
 *   Ce sous-programme lit un caractère à partir du fichier passé en paramètre.
 * PARAMETRES :
 *   file : pointeur sur un descripteur de fichier logique (File Control Bloc)
 * RESULTAT :
 *  valeur (convertie en int) du caractère lu dans le fichier, sinon
 *    -1 : le BCF est NULL, ou le mode d'ouverture est incorrect
 *    -2 : erreur d'entrée-sorties sur le périphérique de stockage
 *    -3 : fin de fichier
 */
int syr1_getc(SYR1_FILE *file) {
  if (file == NULL) return -1;
  if (file->mode != 'r') return -1;

  // test fin de fichier
  if (file->file_offset > (file->descriptor)->size) return -3;

  // lire le caractere courant
  int res = (int) file->buffer[file->block_offset];
  file->block_offset++;
  file->file_offset++;

  // test fin de bloc
  if (file->block_offset * sizeof(char) > IO_BLOCK_SIZE) {

      // lire le block suivant
      int lecture = read_block ((file->descriptor)->alloc[file->current_block], buffer)
      if (lecture < 0) return lecture;

      // maj position
      file->block_offset = 0;
      file->current_block++; 
  }

  return res;
}

/* SYNOPSYS :
 * 	  int syr1_fclose_read(SYR1_FILE* file) {
 * DESCRIPTION :
 *   Ce sous-programme gère la fermeture d'un fichier logique.
 * PARAMETRES :
 *   file : pointeur sur un Bloc de Contrôle Fichier (BCF)
 * RESULTAT :
 *    0 : la fermeture a réussi
 *   -1 : problème pendant la libération du descripteur de fichier logique
 *        (ou le fichier logiques file vaut NULL)
 */
int syr1_fclose_read(SYR1_FILE* file) {
  if (file == NULL) return -1;
  free (file->buffer);
  return free_logical_file (file);
}

