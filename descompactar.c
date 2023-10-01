#include "descompactar.h"

// ponteiro para o arquivo compactado de entrada - char nome_arquivo_entrada contém o nome do arquivo de entrada
int descompact(FILE *compactar, char nome_arquivo_entrada[])
{
    int t_arquivo_descompactado = 0;
    short t_trash, t_arvore = 8191;
    char output_name[50];
    unsigned char primeiro_byte, segundo_byte;

    node *root = NULL;
    FILE *descompactar;

    getchar();
    printf("Enter the output file file:\n");
    scanf("%[^\n]s", output_name);

    compactar = fopen(nome_arquivo_entrada, "rb"); // lê em binário / read binary
    descompactar = fopen(output_name, "wb");       //  escreve em binário / write binary

    fseek(compactar, 0, SEEK_END);              // escreve em binário e vai para a posição final do arquivo
    t_arquivo_descompactado = ftell(compactar); // ftell retorna o tamanho em bytes do arquivo final
    rewind(compactar);                          // Retorna a posição inicial do arquivo

    primeiro_byte = fgetc(compactar);
    segundo_byte = fgetc(compactar);
    t_trash = primeiro_byte >> 5;                                // Mova 5 bits para a direita, deixando apenas os 3 bits de lixo.
    t_arvore = ((primeiro_byte << 8) | segundo_byte) & t_arvore; // t_arvore recebe o tamanho de uma árvore
    root = build_tree(compactar);                                // constrói a árvore huffman

    fseek(compactar, t_arvore + 2, SEEK_SET); // escreva depois da árvore no novo arquivo
    print_byte(compactar, descompactar, root, t_arvore, t_trash, t_arquivo_descompactado);
    fclose(compactar);
    fclose(descompactar);
    printf("DONE!\n");
    return 0;
}

// função recursiva que reconstrói a árvore Huffman a partir dos dados compactados.
node *build_tree(FILE *compactar) // Rebuild the original hufftree by given a string
{
    unsigned char atual;
    node *new_node;
    atual = fgetc(compactar);
    if (atual == '*')
    {
        new_node = create_node(atual);
        new_node->left = build_tree(compactar);
        new_node->right = build_tree(compactar);
    }
    else if (atual == (char)SLASH) // slash == '\' -  Se o caracter for uma barra, significa que um caracter de escape foi usado. Ele lê o próximo caracter e cria um novo nó.
    {
        atual = fgetc(compactar);
        new_node = create_node(atual);
    }
    else
    {
        new_node = create_node(atual);
    }
    return new_node;
}

// cria um novo nó para a árvore Huffman. Pega um unsigned charparâmetro caracter e constrói um novo nó com o caractere fornecido.
node *create_node(unsigned char caractere)
{
    unsigned char *aux = (unsigned char *)malloc(sizeof(unsigned char)); // Aloca  a memória para um novo 'usigned char'
    *aux = caractere;
    node *new_node = (node *)malloc(sizeof(node));
    new_node->frequency = 0;
    new_node->item = aux;
    new_node->right = NULL;
    new_node->left = NULL;
    new_node->next = NULL;
    return new_node;
}

// responsável por ler bits do arquivo compactado, percorrer a árvore Huffman e gravar os caracteres correspondentes no arquivo de saída.
void print_byte(FILE *compactar, FILE *descompactar, node *root, short t_arvore, short t_trash, int t_arquivo_descompactado) // Write the descompactar file
{
    long int i, j, k;
    unsigned char c;
    node *atual = root;
    for (i = 0; i < t_arquivo_descompactado; ++i)
    {
        c = fgetc(compactar);
        if (i == t_arquivo_descompactado - 1)
            k = 8 - t_trash;
        else
            k = 8;
        for (j = 0; j < k; ++j)
        {
            if (is_bit_i_set(c, 7 - j))
                atual = atual->right;
            else
                atual = atual->left;
            if (atual->left == NULL && atual->right == NULL)
            {
                fprintf(descompactar, "%c", *(unsigned char *)atual->item);
                atual = root;
            }
        }
    }
}