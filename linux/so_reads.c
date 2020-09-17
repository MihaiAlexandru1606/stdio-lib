/**
 * @author : mihai
 * @file : so_reads.c
 */
#include "so_file.h"

/**
 *  Citeste nmemb elemente, fiecare de dimensiune size. Datele citite
 *  sunt stocate la adresa de memorie specificata prin ptr.
 * @param ptr aderesa unde se stocheaza datele
 * @param size dimensiunea unui element
 * @param nmemb numar elemente
 * @param stream fisierul deschis
 * @return Intoarce numărul de elemente citite. In caz de eroare sau
 *  daca s-a ajuns la sfarșitul fișierului, funcția intoarce 0.
 */
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	int read_bytes = 0;
	size_t current_read;
	size_t nbytes = size * nmemb;

	if (stream == NULL || ptr == NULL)
		return 0;

	stream->error = 0;
	/** se ferica daca se poate citi */
	if ((stream->open_mod != O_RDONLY) && !(stream->open_mod & O_RDWR)) {
		stream->error = EPERM;
		return 0;
	}

	/** daca sunt date de scris din buffer */
	if (stream->open_mod & O_RDWR && stream->cursor_buffer_write != 0)
		so_fseek(stream, 0, SEEK_CUR);

	/** daca exista deja datele in buffer */
	if (stream->len_read - stream->cursor_buffer_read >= (int)nbytes) {
		memcpy(ptr, stream->buffer + stream->cursor_buffer_read,
		       nbytes);
		stream->cursor_buffer_read += nbytes;
		return nmemb;
	}

	/** scrierea datelor care mai sunt buffer, urmand citirea celorlalte */
	if ((stream->len_read - stream->cursor_buffer_read) > 0) {
		memcpy(ptr, stream->buffer + stream->cursor_buffer_read,
		       (stream->len_read - stream->cursor_buffer_read));

		ptr = (char *)ptr + (stream->len_read -
					stream->cursor_buffer_read);
		read_bytes += (stream->len_read - stream->cursor_buffer_read);
		nbytes -= (stream->len_read - stream->cursor_buffer_read);
		/** resetarea cursor */
		stream->len_read = 0;
		stream->cursor_buffer_read = 0;
	}

	/** citirea datelor efectiva */
	while (nbytes > 0) {
		current_read = read(stream->file_descriptor, stream->buffer,
				    IO_BUFFER_SIZE);

		/** s-a ajuns la sfarsitul fiserului */
		if (current_read == 0) {
			stream->cursor_buffer_read++;
			stream->eof = 1;
			return read_bytes / size;
		}
	/** eroare */
		if ((int)current_read == -1) {
			stream->error = EIO;
			return read_bytes / size;
		}

		/** s-au citit unu numar exact de bytes sau mai mult*/
		/** restul byte-ului raman in buffer pt urmataorea citire */
		if (current_read >= nbytes) {
			memcpy(ptr, stream->buffer, nbytes);
			stream->len_read = current_read;
			stream->cursor_buffer_read = nbytes;
			nbytes = 0;

			continue;
		}

		/** inca mai trebuie sa citim date */
		/** copierea datelor citite */
		memcpy(ptr, stream->buffer, current_read);
		ptr = (char *)ptr + current_read;
		nbytes -= current_read;
		read_bytes += current_read;
	}

	return nmemb;
}

/**
 * citește un caracter din fișier.
 * @param stream fisierul deschis
 * @return caraterul citit (valoarea sa in int) sau SO_EOF in caz de eroare
 */
int so_fgetc(SO_FILE *stream)
{
	unsigned char buffer;
	int rc = so_fread(&buffer, sizeof(unsigned char), 1, stream);

	/** in caz de erorare sau EOF */
	if (rc == 0) {
		stream->error = EIO;
		return SO_EOF;
	}

	stream->error = 0;

	return (int)buffer;
}
