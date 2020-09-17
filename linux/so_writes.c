/**
 * @author : mihai
 * @file : so_writes.c
 */
#include "so_file.h"

/**
 * functia scrie in fisier len bytes  din buffer,
 * @param file_descriptor HANDLE unde se scriu date
 * @param buffer adresa de memorie unde sunt datele
 * @param len dimensiune in bytes
 * @return numarul de byste sau -1 in caz de eroare
 */
size_t write_buffer(int file_descriptor, char *buffer, int len)
{
	int bytes_write = 0;
	size_t current_write;

	while (1) {
		current_write = write(file_descriptor, buffer + bytes_write,
				      len - bytes_write);

		if ((int)current_write == -1)
			return -1;

		bytes_write += current_write;
		if (len == bytes_write)
			break;
	}

	return len;
}

/**
 * Scrie nmemb elemente, fiecare de dimensiune size.
 * @param ptr  adresa de memorie a datelor
 * @param size dimensiunea
 * @param nmemb numarul de elemente
 * @param stream fisierul deschis
 * @return Intoarce numarul de elemente scrise, sau 0 in caz de eroare.
 */
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	size_t nbytes = size * nmemb;
	int write_bytes = 0;

	if (stream == NULL || ptr == NULL)
		return 0;

	stream->error = 0;
	/** verificam daca avem permisunea dea scrie */
	if (!(stream->open_mod & O_WRONLY) && !(stream->open_mod & O_RDWR)) {
		stream->error = EPERM;
		return 0;
	}

	/** daca a fost citete date inaite */
	if (stream->open_mod & O_RDWR && stream->cursor_buffer_read != 0)
		so_fseek(stream, 0, SEEK_CUR);

	/** daca putem sa scriem datele direct in buffer */
	if (IO_BUFFER_SIZE - stream->cursor_buffer_write >= (int)nbytes) {
		memcpy(stream->buffer + stream->cursor_buffer_write, ptr,
		       nbytes);
		stream->cursor_buffer_write += nbytes;

		return nmemb;
	}

	/** daca exista spatiu pentru o parte din date */
	if (IO_BUFFER_SIZE - stream->cursor_buffer_write >= 0) {
		memcpy(stream->buffer + stream->cursor_buffer_write, ptr,
		       IO_BUFFER_SIZE - stream->cursor_buffer_write);

		int rc = write_buffer(stream->file_descriptor, stream->buffer,
				      IO_BUFFER_SIZE);
		if (rc == -1) {
			stream->error = EIO;
			return 0;
		}

		nbytes -= (IO_BUFFER_SIZE - stream->cursor_buffer_write);
		ptr = (char *)ptr + (IO_BUFFER_SIZE -
					stream->cursor_buffer_write);
		write_bytes += (IO_BUFFER_SIZE - stream->cursor_buffer_write);
	}

	while (nbytes > 0) {
		/** s-a umplut bufferul */
		if (nbytes > IO_BUFFER_SIZE) {
			int rc =
			    write_buffer(stream->file_descriptor, (char *)ptr,
					 IO_BUFFER_SIZE);
			/** in caz de eroare */
			if (rc == -1) {
				stream->error = EIO;
				stream->cursor_buffer_write = 0;
				return write_bytes / nbytes;
			}

			nbytes -= IO_BUFFER_SIZE;
			write_bytes += IO_BUFFER_SIZE;
			ptr = (char *)ptr + IO_BUFFER_SIZE;

		} else {
			memcpy(stream->buffer, ptr, nbytes);
			stream->cursor_buffer_write = nbytes;
			nbytes = 0;
		}
	}

	return nmemb;
}

/**
 *  Scrie un caracter în fisier.
 * @param c caraterul de scris
 * @param stream fisierul deschis
 * @return Intoarce caracterul scris sau SO_EOF in caz de eroare.
 */
int so_fputc(int c, SO_FILE *stream)
{
	int rc = so_fwrite(&c, sizeof(unsigned char), 1, stream);

	stream->error = 0;
	/** in caz de eroare */
	if (rc < 1) {
		stream->error = EIO;
		return SO_EOF;
	}

	return c;
}

/**
 * Scrierea datelor din buffer
 * @param stream fiserul deschis
 * @return Intoarce 0 în caz de succes sau SO_EOF in caz de eroare.
 */
int so_fflush(SO_FILE *stream)
{
	stream->error = 0;

	if (stream->cursor_buffer_write == 0)
		return 0;

	size_t rc = write_buffer(stream->file_descriptor, stream->buffer,
				 stream->cursor_buffer_write);
	/** cazul in care nu s-au putut scrie datele */
	if ((int)rc != stream->cursor_buffer_write) {
		stream->error = EIO;
		return SO_EOF;
	}
	stream->cursor_buffer_write = 0;

	return 0;
}
