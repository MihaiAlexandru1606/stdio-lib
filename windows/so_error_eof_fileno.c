/**
 * @author : mihai
 * @file : so_error_eof_fileno.c
 */
#include "so_file.h"

/**
 * functia indica daca s-a ajuns la sfrasitul fisierului
 * acest lucru este detectat daca este EOF, in functia so_fread si
 * restat de so_fseek
 * @param stream fisierul descis
 * @return 0 nu s-a ajuns, 1 s-a ajuns la finalul fisierului
 */
int so_feof(SO_FILE *stream)
{
	stream->error = 0;
	return stream->eof;
}

/**
 * functia retuneza HANDLE asociat pentru un SO_FILE
 * @param stream fisierul descis
 * @return HANDLE pentru respectivul fisier
 */
HANDLE so_fileno(SO_FILE *stream)
{
	stream->error = 0;
	return stream->file_descriptor;
}

/**
 * functia retuneza daca s-a produs o erorare, initial este 0, daca se produce
 * o eroare este setata la o valoare pozitiva
 * @param stream fisierul descis
 * @return 0 daca nu s-a produs nici o erorare, altfe o valoare pozitiva
 */
int so_ferror(SO_FILE *stream)
{
	return stream->error;
}
