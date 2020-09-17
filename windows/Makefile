build: lib

lib: so_open_close.obj so_reads.obj so_writes.obj so_seek_tell.obj so_error_eof_fileno.obj so_popen_pclose.obj
	link /nologo  /dll /out:so_stdio.dll /implib:so_stdio.lib so_open_close.obj so_reads.obj so_writes.obj so_seek_tell.obj so_error_eof_fileno.obj so_popen_pclose.obj

so_open_close.obj : so_open_close.c so_file.h so_stdio.h
	cl /nologo /MD /W3 /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS /c so_open_close.c

so_reads.obj : so_reads.c so_file.h so_stdio.h
	cl /nologo /MD /W3 /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS /c so_reads.c

so_writes.obj : so_writes.c so_file.h so_stdio.h
	cl /nologo /MD /W3 /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS /c so_writes.c

so_seek_tell.obj : so_seek_tell.c so_file.h so_stdio.h
	cl /nologo /MD /W3 /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS /c so_seek_tell.c

so_error_eof_fileno.obj : so_error_eof_fileno.c so_file.h so_stdio.h
	cl /nologo /MD /W3 /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS /c so_error_eof_fileno.c

so_popen_pclose.obj : so_popen_pclose.c so_file.h so_stdio.h
	cl /nologo /MD /W3 /D_CRT_SECURE_NO_DEPRECATE /DDLL_EXPORTS /c so_popen_pclose.c

.PHONY : clean

clean:
	del *.obj *.dll *.lib *.exe *.exp
