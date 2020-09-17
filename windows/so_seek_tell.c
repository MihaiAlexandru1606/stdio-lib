/**
 * @author : mihai
 * @file : so_seek_tell.c
 */
#include "so_file.h"

/**
 * functia muta cursorul fisierului.
 * @param stream fiserul deschis
 * @param offset valoarea aduagata
 * @param whence pozitia specificata
 * @return Intoarce 0 în caz de succes si -1 in caz de eroare.
 */
int so_fseek(SO_FILE *stream, long offset, int whence)
{
	DWORD dwPos;
	int rc;

	stream->error = 0;
	stream->eof = 0;
	/** daca sunt deate de scris in buffer */
	if (stream->cursor_buffer_write != 0) {
		rc = write_buffer(stream->file_descriptor, stream->buffer,
				  stream->cursor_buffer_write);
		if (rc == -1) {
			stream->error = EIO;
			return -1;
		}

		stream->cursor_buffer_write = 0;
	}

	/** daca sunt date citite in buffer, trebui sa modificam offset-ul  */
	/** daca se face referiere la pozitia actuala */
	if (whence == SEEK_CUR &&
	    stream->len_read - stream->cursor_buffer_read != 0)
		offset -= stream->len_read - stream->cursor_buffer_read;

	/** invalidarea datelor din buffer */
	stream->cursor_buffer_read = 0;
	stream->len_read = 0;

	/** mutarea cursorului */
	dwPos = SetFilePointer(stream->file_descriptor, offset, 0, whence);

	if (dwPos == INVALID_SET_FILE_POINTER) {
		stream->error = EINVAL;
		return -1;
	}

	return 0;
}

/**
 * Determina pozitia in fisier a cursorlui
 * @param stream fiserul deschis
 * @return Intoarce poziția curenta din fișier.
 *         In caz de eroare funcția întoarce -1.
 */
long so_ftell(SO_FILE *stream)
{
	/** aflarea pozitiei actuale */
	long position = SetFilePointer(
					stream->file_descriptor,
					0,
					NULL,
					FILE_CURRENT
					);

	stream->error = 0;

	if (position == INVALID_SET_FILE_POINTER) {
		stream->error = EINVAL;
		return position;
	}
	/** daca sunt datate scrise in buffer */
	if (stream->cursor_buffer_write != 0)
		position += stream->cursor_buffer_write;

	/** daca am citi date */
	if (stream->cursor_buffer_read != 0)
		position -= (stream->len_read - stream->cursor_buffer_read);

	return position;
}
