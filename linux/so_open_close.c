/**
 * @author : mihai
 * @file : so_open_close.c
 */
#include "so_file.h"

/**
 * Funcția so_fopen aloca o structura SO_FILE pe care o intoarce. In caz de
 * eroare, functia intoarce NULL.
 * @param pathname reprezintă calea catre un fișier
 * @param mode este un string care determina modul în care va fi deschis
 * fișierul
 * @return structura SO_FILE in caz de succes, altfel NULL
 */
SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *file = calloc(1, sizeof(SO_FILE));

	if (file == NULL)
		return NULL;
	/** deschide fișierul pentru citire; mod "r" */
	if (strcmp(mode, ONLY_READ) == STRING_EQUAL) {
		file->file_descriptor =
		    open(pathname, O_RDONLY, DEFAULT_PERMISSION);

		if (file->file_descriptor < 0) {
			free(file);
			errno = ENOENT;
			return NULL;
		}

		file->open_mod = O_RDONLY;

		return file;
	}
	/** deschide fișierul pentru scriere, mod "w" */
	if (strcmp(mode, ONLY_WRITE) == STRING_EQUAL) {
		file->file_descriptor =
		    open(pathname, O_WRONLY | O_CREAT | O_TRUNC,
			 DEFAULT_PERMISSION);
		if (file->file_descriptor < 0) {
			free(file);

			return NULL;
		}

		file->open_mod = O_WRONLY | O_CREAT | O_TRUNC;

		return file;
	}

	/** deschide fisierul pentru citire si scriere, mod "w+" */
	if (strcmp(mode, WRITE_READ) == STRING_EQUAL) {
		file->file_descriptor =
		    open(pathname, O_RDWR | O_CREAT | O_TRUNC,
			 DEFAULT_PERMISSION);
		if (file->file_descriptor < 0) {
			free(file);

			return NULL;
		}

		file->open_mod = O_RDWR | O_CREAT | O_TRUNC;

		return file;
	}
	/** deschide fisierul pentru citire si scriere, mod "r+" */
	if (strcmp(mode, READ_WRITE) == STRING_EQUAL) {
		file->file_descriptor =
		    open(pathname, O_RDWR, DEFAULT_PERMISSION);

		if (file->file_descriptor < 0) {
			free(file);

			return NULL;
		}

		file->open_mod = O_RDWR;

		return file;
	}
	/** deschide fisierul în modul append, mod "a" */
	if (strcmp(mode, APPEND) == STRING_EQUAL) {
		file->file_descriptor =
		    open(pathname, O_WRONLY | O_CREAT | O_APPEND,
			 DEFAULT_PERMISSION);
		if (file->file_descriptor < 0) {
			free(file);

			return NULL;
		}

		file->open_mod = O_WRONLY | O_CREAT | O_APPEND;
		assert(lseek(file->file_descriptor, 0, SEEK_END) > -1);

		return file;
	}
	/** deschide fiserul în modul append+read, mod "a+" */
	if (strcmp(mode, APPEND_READ) == STRING_EQUAL) {
		file->file_descriptor =
		    open(pathname, O_RDWR | O_CREAT | O_APPEND,
			 DEFAULT_PERMISSION);
		if (file->file_descriptor < 0) {
			free(file);

			return NULL;
		}

		file->open_mod = O_RDWR | O_CREAT | O_APPEND;

		return file;
	}

	/** in cazul in care mu este nici unul dintre moduri */
	free(file);
	return NULL;
}

/**
 * Inchide fișierul primit ca parametru și elibereaza memoria
 * folosita de structura SO_FILE
 * @param stream fisiesrul dechis
 * @return  0 in caz de succes sau SO_EOF in caz de eroare
 */
int so_fclose(SO_FILE *stream)
{
	if (stream == NULL)
		return SO_EOF;

	stream->error = 0;
	/** verificam daca mai exista date nescriese in buffer */
	/** daca da, atunci le scriem */
	if (stream->cursor_buffer_write != 0) {
		int rc = write_buffer(stream->file_descriptor, stream->buffer,
				      stream->cursor_buffer_write);
		if (rc == -1) {
			free(stream);
			return SO_EOF;
		}
	}

	int rc = close(stream->file_descriptor);

	if (rc != 0) {
		stream->error = EBADF;
		return SO_EOF;
	}

	free(stream);

	return 0;
}
