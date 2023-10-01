#include "main.h"

int main()
{
    int opt;
    char namefile[1000];

    // printf("Hello! Welcome to Prog.Huff™\n");
    printf("Selecione a opção desejada:\n%10d - Compactar\n%10d - Descompactar\n%10d - Sair\n", 1, 2, 3);
    scanf("%d", &opt);
    getchar();
    FILE *file;
    switch (opt)
    {
    case 1:
        printf("Por favor, informe o nome do arquivo:\n");
        scanf("%[^\n]s", namefile);
        file = fopen(namefile, "rb");
        compact(file, namefile);
        break;
    case 2:
        printf("Por favor, informe o nome do arquivo:\n");
        scanf("%[^\n]s", namefile);
        file = fopen(namefile, "rb");
        descompact(file, namefile);
        break;
    case 3:
        printf("Até breve!\n");
        return 0;
        break;
    }

    return 0;
}