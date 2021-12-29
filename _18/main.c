#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

void print_options(char *name, struct stat *fs);

char * def_args[2] = {".", NULL};
int main(int argc, char * argv[]){
    char ** dirs;
    if(argc < 2){
        dirs = def_args;
    } else {
        dirs = (argv + 1);
    }

    for(int i = 0; dirs[i] != NULL; i++){
        struct stat fs;
//        Для каждой директории мы вызываем lstat, которая заполняет структуру fs
        if(lstat(dirs[i], &fs) == 0){
            print_options(dirs[i], &fs);
        } else {
            perror(dirs[i]);
        }
    }

    return 0;
}

void print_options(char *name, struct stat *fs){
    char mode[11];
//  Определяем является ли это файлом
    if(S_ISREG(fs->st_mode)){
        mode[0] = '-';
//  Определяем является ли директорией
    } else if(S_ISDIR(fs->st_mode)){
        mode[0] = 'd';
    } else {
        mode[0] = '?';
    }
//  Для st_mode определены следующие значения маски:

//  Есть ли у нас права на чтение
    mode[1] = (fs->st_mode & S_IRUSR) ? 'r' : '-';
//    Есть ли права на редактирование
    mode[2] = (fs->st_mode & S_IWUSR) ? 'w' : '-';
//    Есть ли права на выполнение
    mode[3] = (fs->st_mode & S_IXUSR) ? 'x' : '-';

//  Далее определены все те же права только для группы
    mode[4] = (fs->st_mode & S_IRGRP) ? 'r' : '-';
    mode[5] = (fs->st_mode & S_IWGRP) ? 'w' : '-';
    mode[6] = (fs->st_mode & S_IXGRP) ? 'x' : '-';

//    Далее мы получаем значение прав для других пользователей
    mode[7] = (fs->st_mode & S_IROTH) ? 'r' : '-';
    mode[8] = (fs->st_mode & S_IWOTH) ? 'w' : '-';
    mode[9] = (fs->st_mode & S_IXOTH) ? 'x' : '-';
    mode[10] = 0;
    printf("%s\t", mode);
//  Количесво hard link
    printf("%ld\t", fs->st_nlink);

//    Если искомая запись по user id не найдена то мы печатаем user id, иначе имя пользователя
    struct passwd* uent = getpwuid(fs->st_uid);
    if (uent == NULL){
        printf("%d", fs->st_uid);
    } else {
        printf("%s", uent->pw_name);
    }
    printf("\t");

//    Соответсвенно если мы смогли получить структуру с ифнормацией о группе мы напечатаем имя группы, иначе group id
    struct group* gent = getgrgid(fs->st_gid);
    if (gent == NULL) {
        printf("%d", fs->st_gid);
    } else {
        printf("%s", gent->gr_name);
    }
    printf("\t");

//  Размер файла
    printf("%lu\t", fs->st_size);

    printf("%s\t",  basename(name));
//  st_ctime - время последнего изменения файла, которые мы и печатаем
    printf("%s", ctime(&fs->st_ctime));
}
