/**
 * @author : mihai
 * @file : so_open_close.c
 */
#include "so_file.h"

/**
 * Funcția so_fopen aloca o structura SO_FILE pe care o intoarce. In caz de
 * eroare, functia intoarce NULL.
 * @param pathname reprezintă calea catre un fișier
 * @param mode este un string care determina modul în care va fi deschis fișierul
 * @return structura SO_FILE in caz de succes, altfel NULL
 */
SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *file = calloc(1, sizeof(SO_FILE));
	DWORD dwPos;

	if (file == NULL)
		return NULL;
	/** deschide fișierul pentru citire; mod "r" */
	if (strcmp(mode, ONLY_READ) == STRING_EQUAL) {
		file->file_descriptor = CreateFile(
						pathname,
						GENERIC_READ,
						FILE_SHARE_READ |
						FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_READONLY,
						NULL
						);
		if (file->file_descriptor == INVALID_HANDLE_VALUE) {
			free(file);
			errno = ENOENT;
			return NULL;
		}
		file->open_mod = O_RDONLY;

		return file;
	}
	/** deschide fișierul pentru scriere, mod "w" */
	if (strcmp(mode, ONLY_WRITE) == STRING_EQUAL) {
		file->file_descriptor = CreateFile(
						pathname,
						GENERIC_WRITE,
						FILE_SHARE_READ |
						FILE_SHARE_WRITE,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (file->file_descriptor == INVALID_HANDLE_VALUE) {
			free(file);
			return NULL;
		}
		file->open_mod = O_WRONLY;

		return file;
	}
	/** deschide fisierul pentru citire si scriere, mod "w+" */
	if (strcmp(mode, WRITE_READ) == STRING_EQUAL) {
		file->file_descriptor = CreateFile(
						pathname,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ |
						FILE_SHARE_WRITE,
						NULL,
						CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (file->file_descriptor == INVALID_HANDLE_VALUE) {
			free(file);
			return NULL;
		}
		file->open_mod = O_RDWR;

		return file;
	}
	/** deschide fisierul pentru citire si scriere, mod "r+" */
	if (strcmp(mode, READ_WRITE) == STRING_EQUAL) {
		file->file_descriptor = CreateFile(
						pathname,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ |
						FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (file->file_descriptor == INVALID_HANDLE_VALUE) {
			free(file);
			return NULL;
		}
		file->open_mod = O_RDWR;

		return file;
	}
	/** deschide fisierul în modul append, mod "a" */
	if (strcmp(mode, APPEND) == STRING_EQUAL) {
		file->file_descriptor = CreateFile(
						pathname,
						FILE_APPEND_DATA,
						FILE_SHARE_WRITE |
						FILE_SHARE_READ,
						NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (file->file_descriptor == INVALID_HANDLE_VALUE) {
			free(file);
			return NULL;
		}
		file->open_mod = O_WRONLY;
		dwPos = SetFilePointer(
					file->file_descriptor,
					0,
					NULL,
					FILE_END
					);
		assert(dwPos != INVALID_SET_FILE_POINTER);

		return file;
	}
	/** deschide fiserul în modul append+read, mod "a+"  */
	if (strcmp(mode, APPEND_READ) == STRING_EQUAL) {
		file->file_descriptor = CreateFile(
						pathname,
						FILE_APPEND_DATA |
						FILE_GENERIC_READ,
						FILE_SHARE_WRITE |
						FILE_SHARE_READ,
						NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL
						);
		if (file->file_descriptor == INVALID_HANDLE_VALUE) {
			free(file);
			return NULL;
		}
		file->open_mod = O_RDWR;

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
	BOOL rc;

	if (stream == NULL)
		return SO_EOF;

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

	rc = CloseHandle(stream->file_descriptor);

	if (rc == FALSE) {
		stream->error = EBADF;
		return SO_EOF;
	}

	free(stream);

	return 0;
}
