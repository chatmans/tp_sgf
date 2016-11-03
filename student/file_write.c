#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<syr1_file.h>

/* BINOME : NGUYEN NHON Berenger & POSNIC Antoine
 * Groupe 1.1
 */


/* SYNOPSYS :
 * 	  int syr1_fopen_write(char *name, SYR1_FILE *file) {
 * DESCRIPTION :
 *   Ce sous-programme gère l'ouverture d'un fichier logique en mode écriture.
 * PARAMETRES :
 *   name : chaîne de caratère contenant le nom externe du fichier à ouvrir
 *   file : pointeur sur un Bloc Control Fichier (File Control Bloc)
 * RESULTAT :
 *    0 : ouverture réussie
 *   -1 : autre erreur
 */
int syr1_fopen_write(char *name, SYR1_FILE* file) {

  switch (search_entry (name, & (file->descriptor))) {
	// cas de fichier existant dans le SGF
	case 0: break;
	// cas de fichier non-existant : creer un nouveau fichier
	case -1:
		// creer l'entree dans le catalogue
		if (create_entry (name, & (file->descriptor)) < 0)
			return -1;
		// allocation du premier bloc de donnees
		int ua = get_allocation_unit();
		if (ua < 0) {
			remove_entry (name);
			return -1;
		}
		(file->descriptor).alloc[0] = ua;
		break;
	// erreur d'E/S
	case -2: return -1;
  }
		
  // Allouer en memoire la taille d'un block d'E/S pour
  // le tampon d'E/S
  file->buffer = (unsigned char*) malloc (IO_BLOCK_SIZE);
  if (file->buffer == NULL) return -1;

  // Initialisations
  strcpy (file->mode, "w");
  file->current_block = 0;
  file->file_offset = 0;
  file->block_offset = 0;
      
  return 0;
}

/*
 * SYNOPSYS :
 * 	 int syr1_fwrite(SYR1_FILE *file, int item_size, int nbitem, char* buf)
 * DESCRIPTION :
 *   Ce sous-programme écrit nbitem articles de taille item_size dans le fichier
 *   fichier paramètre à partir du tampon mémoire.
 * PARAMETRES :
 *  	 file : pointeur sur un Bloc Control Fichier (File Control Bloc)
 *  item_size : taille d'un article
 *    nb_item : nombre d'article à lire
 * RESULTAT :
 *    le nombre d'articles effectivement écrits dans le fichier, sinon un code
 *    d'erreur (cf syr1_putc())
 */
int syr1_fwrite(SYR1_FILE* file, int item_size, int nbitem, char *buffer)  {
  int count = 0;
  while (count<nbitem*item_size) {
    int res = syr1_putc(buffer[count],file);
    if (res<0) {
      return res;
    }
    count++;
  }
  return count;
}

/*
 * SYNOPSYS :
 * 	 int syr1_putc(unsigned char c, SYR1_FILE *file)
 * DESCRIPTION :
 *   Ce sous-programme écrit un caractère dans le fichier passé en paramètre.
 * PARAMETRES :
 *   file : pointeur sur un Bloc Control Fichier (File Control Bloc)
 *      c : caractère à écrire
 * RESULTAT :
 *     0 : écriture réussie
 *    -1 : le descripteur de fichier logique est NULL, ou le mode d'ouverture
 *         du fichier passée en paramètre est incorrect
 *    -2 : erreur d'entrée-sorties sur le périphérique de stockage
 *    -3 : taille maximum de fichier atteinte
 *    -4 : plus de blocs disques libres
 */
int syr1_putc(unsigned char c, SYR1_FILE* file)  {
  if (file == NULL) return -1;
  if (strcmp (file->mode, "w") != 0) return -1;

	// test de fin de bloc
	if (file->block_offset == IO_BLOCK_SIZE) {

		// ecrire le contenu du tampon dans le bloc courant
		if (write_block ( (file->descriptor).alloc[file->current_block],
					file->buffer) < 0)
			return -2;

		// se preparer a ecrire dans le bloc suivant
		file->current_block++;
		if (file->current_block == MAX_BLOCK_PER_FILE)
			return -3;

		// se placer en debut de tampon
		file->block_offset = 0;
	}

	// test de fin de fichier
	if (file->file_offset == (file->descriptor).size) {

		// cas ou le caractere doit etre ecrit dans un nouveau bloc
		if ( (file->descriptor).size % IO_BLOCK_SIZE == 0) {

			// obtenir l'adresse d'un bloc libre
			int new_ua = get_allocation_unit();
			switch (new_ua) {

				case -1: return -4;

				case -2: return -2;

				// ajouter l'adresse disque a la table d'allocation
				default:
					(file->descriptor).alloc[file->current_block] = new_ua;
			}		
		}

		// incrementer la taille du fichier
		(file->descriptor).size++;
	}

	// ecrire le caractere a la position courante dans le tampon
	file->buffer[file->block_offset] = c;

	// avancer dans le tampon
	file->block_offset++;
	file->file_offset++;

	return 0;
}



/* SYNOPSYS :
 * 	  int syr1_fclose_write(SYR1_FILE* file) {
 * DESCRIPTION :
 *   Ce sous-programme gère la fermeture d'un fichier logique.
 * PARAMETRES :
 *   file : pointeur sur un Bloc de Contrôle Fichier (BCF)
 * RESULTAT :
 *    0 : la fermeture a réussi
 *   -1 : problème pendant la libération du descripteur de fichier logique
 *        (ou le fichier logiques file vaut NULL)
 *   -2 : erreur d'entrée-sorties sur le périphérique de stockage
 */
int syr1_fclose_write(SYR1_FILE* file) {
  if (file == NULL) return -1;

  // ecrire le contenu du tampon dans le dernier bloc
  if (write_block ( (file->descriptor).alloc[file->current_block],
			file->buffer) < 0) 
	return -2;

  // mettre a jour la taille du fichier
  (file->descriptor).size = file->file_offset;
  
  // mettre a jour l'entree du catalogue
  if (update_entry (& (file->descriptor)) < 0) return -2;

  // liberer les allocations memoire
  if (file->buffer != NULL) free (file->buffer);

  return free_logical_file (file);
}


