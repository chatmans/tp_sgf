NAME = "POSNIC et NGUYEN NHON"
CC = gcc
CFLAGS = -g -Wall -I./include
LDFLAGS = -g

OBJ_DONNES = \
	obj/block_alloc.o \
	obj/directory.o \
	obj/file_alloc.o \
	obj/physical_io.o

OBJ_AFAIRE = \
	obj/openclose.o \
	obj/file_read.o \
	obj/file_write.o 

OBJ = $(OBJ_DONNES) $(OBJ_AFAIRE)

PROGS = \
	bin/simple_read \
	bin/simple_write \
	bin/test_read \
	bin/test_write


INC_FILES = \
	include/physical_io.h \
	include/syr1_file.h \
	include/directory.h 

all: 
	@echo "==============================================================" 
	@echo "usage : make [progs] [clean] ..."
	@echo "  "
	@echo "  progs       : Compilation et EdL des programmes de tests"
	@echo "  clean       : efface les fichiers objets (*.o) et binaires"
	@echo "  init_fs     : regénère un systeme de fichier 'complet'"
	@echo "  inspect     : affiche le disque virtuel avec l'éditeur héxadécimal" 
	@echo "  simple_read : lance le test de lecture avec simple_read"
	@echo "  test_read   : lance le test de lecture avec test_read"
	@echo "  simple_write: lance le test d'écriture avec simple_write"
	@echo "  test_write  : lance le test d'écriture avec test_write"
	@echo "  listing     : Génère un fichier pdf (imprimable) des sources"
	@echo "==============================================================" 
	@echo "  "

progs : $(PROGS)

#
# les fichiers sources du TP à compléter (file_read.c, file_write.c)
#

obj/openclose.o : student/openclose.c $(INC_FILES)
	$(CC) -c  $(CFLAGS) -o obj/openclose.o student/openclose.c

obj/file_read.o : student/file_read.c $(INC_FILES)
	$(CC) -c $(CFLAGS) -o obj/file_read.o student/file_read.c

obj/file_write.o : student/file_write.c $(INC_FILES)
	$(CC) -c  $(CFLAGS) -o obj/file_write.o student/file_write.c

#
# modules de test
#

bin/simple_read: tests/simple_read.c $(OBJ) $(INC_FILES)
	$(CC) -o  bin/simple_read  $(CFLAGS) $(OBJ) tests/simple_read.c 

bin/simple_write : tests/simple_write.c $(OBJ) $(INC_FILES)
	$(CC) -o bin/simple_write $(CFLAGS) $(OBJ) tests/simple_write.c 

bin/test_read: tests/test_read.c $(OBJ) $(INC_FILES)
	$(CC) -o bin/test_read  $(CFLAGS) $(OBJ) tests/test_read.c 

bin/test_write : tests/test_write.c $(OBJ) $(INC_FILES)
	$(CC) -o bin/test_write $(CFLAGS) $(OBJ) tests/test_write.c 

#
# Regénération du SGF
#
init_fs:
	cp -p /share/l3info/syr1/tp_sgf/disk.img ./

#
# Génération du listing en pdf
#
listing:
	@a2ps --tabsize=4 --medium=A4 --title=$(NAME) -o tp_sgf.ps student/file_read.c student/file_write.c 
	@ps2pdf -sPAPERSIZE=a4 tp_sgf.ps

inspect:
	ghex disk.img

# test simple de lecture
simple_read: $(PROGS)
	@./bin/simple_read

# test simple d'écriture 
simple_write: $(PROGS)
	@./bin/simple_write

# test poussé de lecture ; voir résultats attendus dans read.sor
test_read: $(PROGS)
	@./bin/test_read

# test poussé d'écriture ; voir résultats attendus dans write.sor
test_write: $(PROGS)
	@./bin/test_write

#
# Nettoyage des modules
#
clean :
	\rm -f obj/file_read.o obj/file_write.o obj/openclose.o bin/test_* bin/simple_*
