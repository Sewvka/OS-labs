#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define TIME_OUT 5
#define BUF_SIZE 64

static int isTimeout = 0;

void SigAlarmHandler(int sig) {
    isTimeout = 1;
}

void readString(int fd, char *eof, char *buf, int bufsize) {
    if (*eof) {
        return;
    }

    int read_chars_cnt = 0;

//    Ставим Сигнал на 5 секунд
    alarm(TIME_OUT);

    char ch = 0;

//    Пока мы не дойдем до символа переноса строки, считываем по символу и записываем в буффер
    do {
        int res = 0;
        if ((res = read(fd, &ch, sizeof(char))) < 0) {
            if (errno != EINTR || !isTimeout) {
                perror("read err");
                exit(-1);
            }
            errno = 0;
            return;
        }

        if(res == 0){
            break;
        }

        buf[read_chars_cnt++] = ch;

        if(read_chars_cnt + 1 > bufsize){
            break;
        }

    } while (ch != '\n');

    if (read_chars_cnt == 0) {
        *eof = 1;
        return;
    }

//    Выводим все из буфера
    if (write(1, buf, read_chars_cnt) < 0) {
        perror("write err");
        exit(-1);
    }
}

int isEnd(const char *eof, int files_count) {
    for (int i = 0; i < files_count; ++i) {
        if (!eof[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "not enough arguments");
        exit(1);
    }

    int files_count = 0;

//    Массив файловых дискрипторов
    int fds[argc-1];
//    Флаги конца файла
    char eof[argc-1];

//  Открываем файлы и записываем файловые дискрипторы в массив
    for (int i = 1; i < argc; ++i) {
        int fd = open(argv[i], O_RDONLY | O_NDELAY);
        if (fd < 0) {
            perror(argv[i]);
            return -1;
        }
        fds[files_count++] = fd;
    }

//    Заполняем нулями массив концов файлов
    for (int i = 0; i < files_count; ++i) {
        eof[i] = 0;
    }

    char buf[BUF_SIZE];

    signal(SIGALRM, SigAlarmHandler);

    while (1) {
//        Пока не пройдемся по всем файлам начинаем их читать
        for (int i = 0; i < files_count; ++i) {
            readString(fds[i], &eof[i], buf, BUF_SIZE);
        }
//        Если мы прочитали все файлы - выход
        if (isEnd(eof, files_count)) {
            break;
        }
    }

    return 0;
}
