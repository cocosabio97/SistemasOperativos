#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int nCopy = 0;
	int c;
	while((nCopy < nBytes) && (c = getc(origin)) != EOF){
		putc((unsigned char)c, destination);
		nCopy++;
	}


	return nCopy;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	char *buffer;
	int c, size = 0;
	//Hacemos un bucle para ver hasta dónde tenemos que leer.
	do{
		c = fgetc(file);
		size++;
	}while((c != (int)'\0') && (c != EOF));

	fseek(file, -size, SEEK_CUR); //Devolvemos el cursor al principio.
	buffer = (char*)malloc(size* sizeof(char));
	fread(buffer, sizeof(char), size, file); //Leemos el file y guardamos en el buffer.

	return buffer;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry *header = (stHeaderEntry *)malloc(sizeof(stHeaderEntry) * *nFiles);

	for(int i = 0; i < *nFiles; ++i){
		header[i].name = loadstr(tarFile);
		fread(&header[i].size, sizeof(int), 1, tarFile);
	}

	return header;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE *tarFile, *inputFile;
	unsigned int headerSize;
	stHeaderEntry *header;

	//Si no hay archivos, mostramos error y salimos
	if(nFiles <= 0){
		fprintf(stderr, "%s", use);
		return EXIT_FAILURE;
	}

	//Abrimos el tarFile.
	//Aumentamos el tamaño de la cabacera a 4(para guadrar nFiles)
	//Reservamos memoria para la cabacera.
	tarFile = fopen(tarName, "w");
	headerSize = sizeof(int);
	header = (stHeaderEntry *)malloc(sizeof(stHeaderEntry) * nFiles);

	//Creamos la cabacera.

	//Para cada nombre de fichero, reservamos espacio de tamaño el nombre de fichero + 1(para '\0')
	//A la vez, lo guardamos en la cabecera.
	// Aumentamos el tamaño de la cabecera para cada par (nombre, tamaño).
	for(int i = 0; i < nFiles; ++i){
		int nameSize = strlen(fileNames[i]) + 1;
		header[i].name = (char *) malloc(nameSize); 
		strcpy(header[i].name, fileNames[i]);
		headerSize += nameSize + sizeof(header -> size);
	}

	//Movemos el cursor del tarFile headerSize posiciones (nos saltamos la cabacera).
	//Abrimos el fichero i y lo escbrimos en tarFile. El numero de bytes escrito lo guardamos en la cabacera. /
	fseek(tarFile, headerSize, SEEK_SET);
	for(int i = 0; i < nFiles; ++i){
		inputFile = fopen(fileNames[i], "r");
		header[i].size = copynFile(inputFile, tarFile, INT_MAX);
		fclose(inputFile);
	}

	//Devolvemos el cursor al principio del tarFile.
	//Escribimos el nFiles y la cabecera.
	rewind(tarFile);
	fwrite(&nFiles, sizeof(int), 1, tarFile);
	for(int i = 0; i < nFiles; ++i){
		fwrite(header[i].name, sizeof(char), strlen(header[i].name) + 1, tarFile);
		fwrite(&header[i].size, sizeof(header[i].size), 1, tarFile);
	}

	fprintf(stdout, "mtar file created successfully\n"); //Mostramos mensaje de éxito.

	//Liberamos memoria
	for(int i = 0; i < nFiles; ++i){
		free(header[i].name); //Para cada nombre de fichero de la cabacera, liberamos memoria.
	}
	free(header); //Liberamos la cabacera.
	fclose(tarFile); //Cerramos el fichero tar.
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	FILE *tarFile, *fich;
	int nFiles;
	stHeaderEntry *header;

	tarFile = fopen(tarName, "r");
	fread(&nFiles, sizeof(int), 1, tarFile);
	header = readHeader(tarFile, &nFiles);

	for(int i = 0; i < nFiles; ++i){
		fich = fopen(header[i].name, "w");
		fprintf(stdout, "[%i]: Creando archivo %s, %i bytes...", i, header[i].name, header[i].size);
		copynFile(tarFile, fich, header[i].size);
		fprintf(stdout, " Ok.\n");
		free(header[i].name);
		fclose(fich);
	}
	free(header);
	fclose(tarFile);
	return EXIT_SUCCESS;
}
