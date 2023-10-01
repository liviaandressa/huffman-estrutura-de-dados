#include "compactar.h"

FILE *compact(FILE *arquivo_original, char nome_arquivo[])
{
    int *freq, count = 0;
    short arvore, trash, final;
    char huff_tree_path[256];
    unsigned char curr_byte, compact = 0;

    node *root = NULL;
    queue *queue = create_queue();
    ht *hash = create_hash();
    FILE *final_file;

    arquivo_original = fopen(nome_arquivo, "rb"); // lê o arquivo original
    printf("Lendo arquivo original...\n");
    freq = save_freq(arquivo_original); // salva frequência em um array com a tabela ASCII
    printf("FEITA!\n");
    rewind(arquivo_original); // Retorna a posição inicial do arquivo

    printf("Gerando fila de frequência...\n");
    frequency_enqueue(freq, queue); // Crie uma lista ordenada por frequência
    printf("FEITA!\n");

    printf("Gerando árvore Huffman...\n");
    root = merge_nodes(queue); // Cria a árvore huffman
    generate_huffman_tree(root, 0, hash, huff_tree_path);
    printf("FEITA!\n");

    printf("Criando arquivo compactado...\n");
    nome_arquivo = strtok(nome_arquivo, ".");
    nome_arquivo = strcat(nome_arquivo, ".huff");
    final_file = fopen(nome_arquivo, "wb");
    printf("FEITA!\n");

    trash = trash_size(hash, freq);
    arvore = tree_size(root);

    trash = trash << 13; // Deixa apenas 3 bits para o conjunto de lixo
    final = trash | arvore;

    curr_byte = final >> 8; // Escreve o primeiro byte do novo arquivo
    fprintf(final_file, "%c", curr_byte);

    curr_byte = final; // Escreva o segundo byte do novo arquivo
    fprintf(final_file, "%c", curr_byte);

    fprint_tree(root, final_file);           // Escreve a árvore no arquivo compactado
    fseek(final_file, 2 + arvore, SEEK_SET); // Escreva depois da árvore no arquivo compactado

    printf("Gravando arquivo compactado...\n");

    // troquei para fgetc
    while (!fgetc(arquivo_original)) // Escreva todos os bytes no arquivo compactado  - lê bytes do arquivo_original até que o final do arquivo seja alcançado
    {
        curr_byte = fgetc(arquivo_original); // lê um byte do arquivo_original e armazena
        int i;
        for (i = 0; i < hash->table[(int)curr_byte]->n; ++i)
        {
            // se o bit atual no código Huffman for '1' defina o bit correspondente no compactbyte usando a set_bit
            if (hash->table[(int)curr_byte]->string[i] == '1')
                compact = set_bit(compact, 7 - count);
            count++;
            if (count == 8)
            {
                fprintf(final_file, "%c", compact);
                compact = 0;
                count = 0;
            }
        }
    }
    printf("FEITA!\n");

    fclose(arquivo_original);
    fclose(final_file);

    return final_file;
}

// calcula a frequência de cada caractere no arquivo de entrada
int *save_freq(FILE *file)
{
    int *freq = (int *)calloc(256, sizeof(int));
    unsigned char c;
    while (!feof(file)) // lê caracteres do arquivo de entrada até que o final do arquivo seja alcançado
    {
        c = fgetc(file);
        freq[(int)c]++;
    }

    return freq;
}

int generate_huffman_tree(node *root, int count, ht *ht, char string[]) // Recieves root of huff tree, count of nodes in preorder, empty hash, empty huff way
{
    if (root->left == NULL && root->right == NULL)
    {
        add_hash(ht, root->item, string, count); // Cria um hash que terá o caminho do byte compactado
    }
    else
    {
        string[count] = '0';
        generate_huffman_tree(root->left, count + 1, ht, string);
        string[count] = '1';
        generate_huffman_tree(root->right, count + 1, ht, string);
    }
}

// calcula o tamanho do lixo
int trash_size(ht *hash, int *frequency)
{
    int i = 0, sum = 0;

    while (i < 256)
    {
        if (frequency[i] != 0)
            sum += (hash->table[i]->n) * frequency[i]; // Diz quantos bits o arquivo final tem
        i++;
    }

    return 8 - (sum % 8); // Diz o tamanho do lixo

    /*
    O loop percorre todas as possíveis entradas (caracteres) e calcula o número total de bits
    necessários para armazenar os caracteres no arquivo compactado.
    Para cada caractere com uma frequência não nula,
    a quantidade de bits é calculada multiplicando o número de bits no código Huffman
    (hash->table[i]->n) pela frequência do caractere (frequency[i]).
    */
}

/*
 return 8 - (sum % 8);

calcula o tamanho do lixo subtraindo o resto da divisão de sum por 8 de 8.
Isso é feito para garantir que o arquivo compactado tenha um número inteiro de bytes.
Se o total de bits (sum) for um múltiplo de 8, nenhum lixo é necessário (o resultado será 8 - 0 = 8).
Se não for um múltiplo de 8, o lixo será o número de bits necessários para preencher o último byte até 8 bits.
*/