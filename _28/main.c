#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>

int main() {
//    Устанавливем начало последовательности
    srand(time(NULL));

    FILE * fd[2];
//    Открываем програмный канал и выполняем sort с ключем для чисел
    if(p2open("sort -n", fd) == -1){
        perror("p2open failed");
        return -1;
    }

//    Выводим в поток 100 рандомных чисел
    for(int i = 0; i < 100; i ++){
        int cur = rand() % 100;
        fprintf(fd[0], "%d\n", cur);
    }

//    Получаем 100 чисел и сортируем их
    fclose(fd[0]);
    printf("After sorting:\n");
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            int cur;
            fscanf(fd[1], "%d", &cur);
            printf("%3d ", cur);
        }
        printf("\n");
    }

    if(pclose(fd[1]) == -1){
        perror("error in pclose");
        return -1;
    }
    return 0;
}
