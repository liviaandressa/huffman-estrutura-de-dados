#include "compactar.h"

/*
 * Auxiliary Functions
 */

// Função de comparação para verificar se dois itens são iguais.
int comparing(void *item_a, unsigned char item_b)
{
    return (*((unsigned char *)item_a) == item_b);
}

// Verifica se o i-ésimo bit em c está definido.
int is_bit_i_set(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask & c;
}

// Define o i-ésimo bit em c como 1.
unsigned char set_bit(unsigned char c, int i)
{
    unsigned char mask = 1 << i;
    return mask | c;
}

// Imprime a representação binária de um caractere.
void print_un_char(unsigned char c)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        if (is_bit_i_set(c, 7 - i))
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
    printf("\n");
}

// Imprime a lista de frequências.
void print_freq_list(int freq[])
{
    int i;
    for (i = 0; i < 256; i++)
    {
        if (freq[i] != 0)
        {
            printf("(%d) %c -> [%d]\n", i, (char)i, freq[i]);
        }
    }
}

/*
 * Priority Queue
 */

// Cria uma nova fila de prioridade.
queue *create_queue()
{
    queue *new_queue = malloc(sizeof(queue));
    new_queue->head = NULL;
    return new_queue;
}

// Adiciona um item à fila de prioridade com base em sua frequência.
void enqueue(queue *queue, void *item, int frequency)
{
    node *new_node = malloc(sizeof(node));
    new_node->item = item;
    new_node->frequency = frequency;
    new_node->left = NULL;
    new_node->right = NULL;
    // Se a fila estiver vazia ou a frequência do novo nó for menor ou igual à cabeça da fila.
    if ((is_empty(queue)) || (frequency <= queue->head->frequency))
    {
        new_node->next = queue->head;
        queue->head = new_node;
    }
    else
    {
        node *current = queue->head;
        // Encontra o local adequado para inserção com base na frequência.
        while ((current->next != NULL) && (current->next->frequency < frequency))
        {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

// Adiciona um nó na sfila de prioridade.
void enqueue_node(node *head, queue *queue)
{
    if ((is_empty(queue)) || (head->frequency <= queue->head->frequency))
    {
        head->next = queue->head;
        queue->head = head;
    }
    else
    {
        node *current = queue->head;
        while ((current->next != NULL) && (current->next->frequency < head->frequency))
        {
            current = current->next;
        }
        head->next = current->next;
        current->next = head;
    }
}

// Remove e retorna o nó com a menor frequência da fila de prioridade.
node *dequeue(queue *queue)
{
    if (is_empty(queue))
    {
        printf("Priority Queue Underflow\n");
    }
    else
    {
        node *node = queue->head;
        queue->head = queue->head->next;
        node->next = NULL;
        return node;
    }
}

// Imprime os itens na fila de prioridade.
void print_queue(queue *queue)
{
    if (!is_empty(queue))
    {
        node *current = queue->head;
        while (current != NULL)
        {
            printf("%c ", *(unsigned char *)current->item);
            current = current->next;
        }
        printf("\n");
    }
}

// Verifica se a fila de prioridade está vazia.
int is_empty(queue *queue)
{
    return (queue->head == NULL);
}

// Adiciona itens à fila de prioridade com base nas frequências.
void frequency_enqueue(int frequency[], queue *queue)
{
    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] != 0)
        {
            unsigned char *aux = (unsigned char *)malloc(sizeof(unsigned char));
            *aux = i;
            enqueue(queue, aux, frequency[i]);
        }
    }
}

/*
 * Binary Tree
 */

// Imprime a árvore binária.
void print_tree(node *root)
{
    if (root != NULL)
    {
        printf("node: %c %d\n", *(unsigned char *)root->item, root->frequency);
        print_tree(root->left);
        print_tree(root->right);
    }
}

// Funde os nós da fila de prioridade para formar uma árvore de Huffman.
node *merge_nodes(queue *queue)
{
    if (queue->head->next != NULL)
    {
        unsigned char *aux = (unsigned char *)malloc(sizeof(unsigned char));
        *aux = '*';
        node *new_node = malloc(sizeof(node));
        new_node->left = dequeue(queue);
        new_node->right = dequeue(queue);
        new_node->item = aux;
        new_node->frequency = new_node->left->frequency + new_node->right->frequency;
        enqueue_node(new_node, queue);
        merge_nodes(queue);
    }
    else
    {
        return queue->head;
    }
}

// Calcula o tamanho da árvore.
int tree_size(node *root)
{
    if (root->left == NULL && root->right == NULL)
    {
        if (comparing(root->item, '*') || comparing(root->item, (char)SLASH))
        {
            return 2;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1 + tree_size(root->left) + tree_size(root->right);
    }
}

// Imprime a árvore em um arquivo compactado.
void fprint_tree(node *root, FILE *compact_file)
{
    if (root != NULL)
    {
        if (root->left == NULL && root->right == NULL && (comparing(root->item, '*') || comparing(root->item, (char)SLASH)))
        {
            fprintf(compact_file, "%c", (char)SLASH);
        }
        fprintf(compact_file, "%c", *(unsigned char *)root->item);
        fprint_tree(root->left, compact_file);
        fprint_tree(root->right, compact_file);
    }
}

/*
 * Hash Table
 */

// Cria uma nova tabela de hash.
ht *create_hash()
{
    ht *new_hash = (ht *)malloc(sizeof(ht));
    for (int i = 0; i < 256; i++)
    {
        new_hash->table[i] = NULL;
    }
    return new_hash;
}

// Cria um novo elemento para a tabela de hash.
element *create_element()
{
    element *new_element = (element *)malloc(sizeof(element));
    new_element->n = 0;
    return new_element;
}

// Adiciona um item à tabela de hash.
void add_hash(ht *ht, void *item, char bit[], int n)
{
    int h = *(unsigned char *)item;
    element *new_element = create_element();
    new_element->n = n;
    for (int i = 0; i < n; i++)
    {
        new_element->string[i] = bit[i];
    }
    ht->table[h] = new_element;
}

// Imprime a tabela de hash.
void print_hash(ht *ht)
{
    for (int i = 0; i < 256; i++)
    {
        if (ht->table[i] != NULL)
        {
            printf("%d ->", i);
            for (int j = 0; j < ht->table[i]->n; j++)
            {
                printf("%c", ht->table[i]->string[j]);
            }
            printf(" [%d]\n", ht->table[i]->n);
        }
    }
}
