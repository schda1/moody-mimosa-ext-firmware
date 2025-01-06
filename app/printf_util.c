#include "main.h"

#include <sys/types.h>
#include <sys/stat.h>

extern UART_HandleTypeDef* hlpuart1;

/*
 * In order to use printf of the newlib library, the _write function 
 * has to be implemented. Implementing the __io_putchar is not enough
 * if the syscalls.c is not added to the project!
 */
int _write(int file, char *data, int len) 
{
    HAL_UART_Transmit(hlpuart1, (uint8_t *)data, len, HAL_MAX_DELAY);
    return len;
}

int _kill(int pid, int sig) 
{
    (void)pid;
    (void)sig;
    return -1;
}

int _getpid(void) 
{
    return 1; 
}

int _close(int file) 
{ 
    return -1; 
}

int _lseek(int file, int ptr, int dir) 
{ 
    return 0; 
}

int _read(int file, char *ptr, int len) 
{ 
    return 0; 
}

int _fstat(int file, struct stat *st) 
{ 
    st->st_mode = S_IFCHR; 
    return 0; 
}

int _isatty(int file) 
{ 
    return 1; 
}

void *_sbrk(ptrdiff_t incr) 
{
    extern char _end;
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;
    heap_end += incr;
    
    return (void *)prev_heap_end;
}

void _exit(int status) 
{
    while (1) {
        __asm__("nop");
    }
}