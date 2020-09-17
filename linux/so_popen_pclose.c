/**
 * @author : mihai
 * @file : so_popen_pclose.c
 */
#include "so_file.h"

/**
 * Functia lanseaza un proces nou, care va executa comanda specificata
 *  de parametrul command
 * @param command comanda de executat
 * @param type modul in care este deschis pipe-ul
 * @return structura SO_FILE in caz de succes, altfel NULL
 */
SO_FILE *so_popen(const char *command, const char *type)
{
	int filedes[2];
	pid_t cpid;
	int rc;

	/** daca este o comanda un type valid */
	if (strcmp(type, ONLY_READ) != STRING_EQUAL &&
	    strcmp(type, ONLY_WRITE) != STRING_EQUAL)
		return NULL;

	/** creare a pipe-ului */
	rc = pipe(filedes);
	if (rc == -1)
		return NULL;

	/** creare procesului copil */
	cpid = fork();
	if (cpid == -1) {
		close(filedes[PIPE_READ]);
		close(filedes[PIPE_WRITE]);

		return NULL;
	} else if (cpid != 0) {
		/** procesul pariunte */
		SO_FILE *file = calloc(1, sizeof(SO_FILE));

		/** eroare la alocae memoriei */
		if (file == NULL) {
			assert(wait(NULL) != -1);
			return NULL;
		}

		file->pid = cpid;
		if (strcmp(type, ONLY_READ) == STRING_EQUAL) {
			file->open_mod = O_RDONLY;
			file->file_descriptor = filedes[PIPE_READ];
			assert(close(filedes[PIPE_WRITE]) == 0);

			return file;
		}

		if (strcmp(type, ONLY_WRITE) == STRING_EQUAL) {
			file->open_mod = O_WRONLY;
			file->file_descriptor = filedes[PIPE_WRITE];
			assert(close(filedes[PIPE_READ]) == 0);

			return file;
		}

	} else {
		/** procusul copil */
		char *const args[] = { BASH, OPTION, (char *const)command,
					NULL };
		/** redirectare intrari, iesiri standard in functie de type */
		if (strcmp(type, ONLY_READ) == STRING_EQUAL) {
			rc = dup2(filedes[PIPE_WRITE], STDOUT_FILENO);

			assert(close(filedes[PIPE_WRITE]) == 0);
			assert(close(filedes[PIPE_READ]) == 0);

			if (rc == -1)
				return NULL;

		}

		if (strcmp(type, ONLY_WRITE) == STRING_EQUAL) {
			rc = dup2(filedes[PIPE_READ], STDIN_FILENO);

			assert(close(filedes[PIPE_WRITE]) == 0);
			assert(close(filedes[PIPE_READ]) == 0);

			if (rc == -1)
				return NULL;
		}

		assert(execv(_PATH_BSHELL, args) != -1);
	}

	return NULL;
}

/**
 * Așteapta terminarea procesului lansat de so_popen și elibereaza memoria
 *  ocupata de structura SO_FILE
 * @param stream fisierul deschis
 * @return  Intoarce codul de iesire al procesului
 */
int so_pclose(SO_FILE *stream)
{
	int status;

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
	/** inchiderea pipe-ului */
	assert(close(stream->file_descriptor) == 0);
	/** asteptam ca prcesul copil sa termine */
	assert(waitpid(stream->pid, &status, 0) != -1);
	free(stream);

	return status;
}
