#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syr1_file.h>

static int calculer_blocs(int size);

extern int physical_io_count;
extern int physical_read_count;
extern int physical_write_count;

int main(int argc, char **argv)
{
  int icar = 0, nbcar_aecrire = 0, lgseq = 15;
  char nom_fichier[FILENAME_SIZE], carinit = '!';
  
  printf("\n***********************************************************\n");
  printf("Test écriture avec changement de nombre de blocs\n");
  printf("Donnez le nom du fichier existant puis le nombre de caractères à réécrire\n");
  scanf("%15s%d", nom_fichier, & nbcar_aecrire);
  printf("Réécriture de %d caractères dans le fichier %s\n", nbcar_aecrire, nom_fichier);

  SYR1_FILE * file_w = syr1_fopen(nom_fichier, "w");
  if (file_w == NULL) {
    printf("[FAILED] Erreur d'ouverture en écriture pour %s\n", nom_fichier);
  }
  else {
    printf("[OK]	Ouverture en écriture réussie\n");

    /* Indiquer si augmentation ou diminution nombre de blocs */
    int anciens_blocs  = calculer_blocs(file_w->descriptor.size);
    int nouveaux_blocs = calculer_blocs(nbcar_aecrire);
    printf("L'ancien   fichier occupe %d blocs\n", anciens_blocs);
    printf("Le nouveau fichier occupe %d blocs\n", nouveaux_blocs);
    int diff = nouveaux_blocs - anciens_blocs; 
    if (diff > 0) {
      printf("==> %d blocs supplémentaires seront alloués\n", diff);
    }
    else if (diff < 0) {
      printf("==> %d blocs seront libérés\n", -diff);
    }
    else {
    }

    /* écrire le nouveau contenu */
    for (icar = 0; icar < nbcar_aecrire; icar++) {
      int res = syr1_putc(carinit + (icar % lgseq), file_w);
      if (res != 0) {
	printf("[FAILED] Problème pendant l'écriture du caractère %d (%c) : code erreur n°%d\n",
	       icar, carinit + (icar % lgseq), res);
      }
    }

    int res = syr1_fclose(file_w);
    if (res == 0) {
      printf("[OK] 	  Fermeture du fichier réussie\n");
    }
    else {
      printf("[FAILED] Problème pendant la fermeture : code erreur n°%d\n",res);
    }
  }

  fflush(stdout);
  printf("\nNombre d'E/S physiques : %d (%d lectures + %d écritures)\n",
	 physical_io_count, physical_read_count,physical_write_count);

  /* relire le fichier */
  {
    printf("relecture du fichier %s\n", nom_fichier);
    SYR1_FILE * file_r = syr1_fopen(nom_fichier, "r");
    if (file_r == NULL) {
      printf("[FAILED] Erreur d'ouverture en lecture pour %s\n", nom_fichier);
    }
    else {
      int nbcar_lus = 0, carlu, nberr = 0;
      printf("[OK]	Ouverture en lecture réussie\n");
      carlu = syr1_getc(file_r);
      while (carlu >= 0) {
	if (carlu != carinit + (nbcar_lus % lgseq)) {
	  printf("[FAILED] Erreur de relecture caractère N° %d : '%c/%02X' attendu mais '%c/%02X' lu\n",
		 nbcar_lus,
		 carinit + (nbcar_lus % lgseq), carinit + (nbcar_lus % lgseq),
		 (char) carlu, (char) carlu
		 );
	  ++nberr;
	}
	++nbcar_lus;
	carlu = syr1_getc(file_r);
      }
      if (nberr == 0) {
	printf("[OK] test relecture %s réussi\n", nom_fichier);
      }
      else {
	printf("[FAILED] test relecture %s a échoué\n", nom_fichier);
      }
      syr1_fclose(file_r);
    }
  }

  return 0;
}

/* calculer le nombre de blocs pour une taille de fichier donnée */
static int calculer_blocs(int size)
{
  int nbBlocs = size / IO_BLOCK_SIZE;
  if (size % IO_BLOCK_SIZE != 0) { ++nbBlocs; }
  return nbBlocs;
}
