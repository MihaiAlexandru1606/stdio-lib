/**
 * @author : mihai
 * @file : so_file.h
 */

#ifndef SO_TEMA_2_SO_FILE_H
#define SO_TEMA_2_SO_FILE_H

#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "so_stdio.h"

#define STRING_EQUAL        0

#define ONLY_WRITE          "w"
#define ONLY_READ           "r"
#define WRITE_READ          "w+"
#define READ_WRITE          "r+"
#define APPEND              "a"
#define APPEND_READ         "a+"

#define IO_BUFFER_SIZE      4096

#define COMMAND_SIZE        256
#define CMD                 "cmd /C "

#pragma pack(1)
struct _so_file {
	HANDLE file_descriptor;  /** HANDLE asociat fiecaruil SO_FILE */
	PROCESS_INFORMATION child_pi; /** infoamtiele despre procesul copil */
	int open_mod; /** modul in care a fost descis fisierul */
	char buffer[IO_BUFFER_SIZE]; /** bufferul pt citire/scriere */
	int cursor_buffer_read; /**indica unde citim din buffer */
	int len_read; /** lungimea datelor citite cu read */
	int cursor_buffer_write; /** inidica unde scriem in buffer */
	int error; /** inidica daca s-a produs o eroare */
	int eof; /** indica daca s-a ajuns la finalul fiserului */
};

#pragma pack()

size_t write_buffer(HANDLE file_descriptor, char *buffer, int len);

#endif				/* SO_TEMA_2_SO_FILE_H */
