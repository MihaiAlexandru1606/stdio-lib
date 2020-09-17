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
	SO_FILE *file;
	BOOL bRet;
	char commnad_process[COMMAND_SIZE] = CMD;
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	file = calloc(1, sizeof(SO_FILE));
	if (file == NULL)
		return NULL;
	/** scriearea comenzi */
	strncat_s(commnad_process, COMMAND_SIZE, command, strlen(command));
	ZeroMemory(&sa, sizeof(sa));
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	sa.bInheritHandle = TRUE;

	/** crearea pipe-ului */
	bRet = CreatePipe(&hRead, &hWrite, &sa, 0);
	if (bRet == FALSE) {
		free(file);
		return NULL;
	}

	si.cb = sizeof(si);
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.dwFlags |= STARTF_USESTDHANDLES;

	/** redirectare intrari, iesiri standard in functie de type */
	if (strcmp(type, ONLY_READ) == STRING_EQUAL) {
		file->file_descriptor = hRead;
		file->open_mod = O_RDONLY;
		si.hStdOutput = hWrite;
		bRet = SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

		if (bRet == FALSE)
			goto ERROR_POPEN;

	} else if (strcmp(type, ONLY_WRITE) == STRING_EQUAL) {
		file->file_descriptor = hWrite;
		file->open_mod = O_WRONLY;
		si.hStdInput = hRead;
		bRet = SetHandleInformation(hWrite, HANDLE_FLAG_INHERIT, 0);

		if (bRet == FALSE)
			goto ERROR_POPEN;

	} else
		goto ERROR_POPEN;

	bRet = CreateProcess(
			NULL,
			commnad_process,
			NULL,
			NULL,
			TRUE,
			0,
			NULL,
			NULL,
			&si,
			&pi
			);

	if (bRet == FALSE)
		goto ERROR_POPEN;

	/** se inchid capetele ne folosite */
	if (strcmp(type, ONLY_READ) == STRING_EQUAL)
		assert(CloseHandle(hWrite) != FALSE);
	else if (strcmp(type, ONLY_WRITE) == STRING_EQUAL)
		assert(CloseHandle(hRead) != FALSE);

	file->child_pi = pi;

	return file;

	/** in caz ca apare o eroare */
ERROR_POPEN:
	CloseHandle(hRead);
	CloseHandle(hWrite);
	free(file);

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
	DWORD dwRes;
	BOOL bRes;
	int rc;

	/** verificam daca mai exista date nescriese in buffer */
	/** daca da, atunci le scriem */
	if (stream->cursor_buffer_write != 0) {
		rc = write_buffer(stream->file_descriptor, stream->buffer,
				  stream->cursor_buffer_write);
		if (rc == -1)
			errno = EIO;
	}
	/** inchiderea pipe-ului */
	assert(CloseHandle(stream->file_descriptor) != FALSE);

	/** asteptam ca prcesul copil sa termine */
	dwRes = WaitForSingleObject(stream->child_pi.hProcess, INFINITE);
	assert(dwRes != WAIT_FAILED);

	/** codul de iesire al procesului copil */
	bRes = GetExitCodeProcess(stream->child_pi.hProcess, &dwRes);
	assert(bRes != FALSE);
	/** inchiderea procesului */
	assert(CloseHandle(stream->child_pi.hProcess) != FALSE);
	assert(CloseHandle(stream->child_pi.hThread) != FALSE);

	free(stream);

	return dwRes;
}
