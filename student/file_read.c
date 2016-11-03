#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<syr1_file.h>
#include<physical_io.h>

/* BINOME : NGUYEN NHON Berenger & POSNIC Antoine
 * Groupe 1.1
 */

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

  if (search_entry (name, & (file->descriptor)) < 0)
	return -1;

	// Allouer en memoire la taille d'un block d'E/S pour
	// le tampon d'E/S
	file->buffer = (unsigned char*) malloc (IO_BLOCK_SIZE);
	if (file->buffer == NULL) return -1;
      
	// Copier le premier bloc dans le buffer
	if (read_block ( (file->descriptor).alloc[0], file->buffer) < 0)
		return -1;
	
	// Initialisations
	strcpy (file->mode, "r");
	file->current_block = 0;
	file->file_offset = 0;
	file->block_offset = 0;

	return 0;

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

  // Cas de BCF NULL
  if (file == NULL) return -1;

  // Cas de mode d'ouverture incorrect
  if (strcmp (file->mode, "r") != 0) return -1;

  // Test de fin de fichier
  if ((file->file_offset) == ((file->descriptor).size)) return -3;

  // Lire le caractere courant
  int res = (int) file->buffer[file->block_offset];

  // Avancer au caractere suivant dans le tampon
  file->block_offset++;
  file->file_offset++;

  // Test de fin de bloc
  if (file->block_offset == IO_BLOCK_SIZE) {

      // lire le bloc suivant
      file->current_block++;
      if (read_block ((file->descriptor).alloc[file->current_block], file->buffer) < 0)
      	return -2;

      // maj position
      file->block_offset = 0;
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

  // Liberer la memoire allouee pour le tampon d'E/S
  if (file->buffer != NULL) free (file->buffer);

  return free_logical_file (file);
}

