//Abdi Abera
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <getopt.h>

#define BUFFER_SIZE 1024

// Structure for Huffman Tree Node
typedef struct Node {
    char theData;
    int freQ;
    struct Node *left, *right;
} Node;

// Function to create a new node with frequency (freQ)
Node *huffmancreateNode(char theData, int freQ) {
    Node *huffnewNode = (Node *)malloc(sizeof(Node));
    huffnewNode->theData = theData;
    huffnewNode->freQ = freQ;
    huffnewNode->left = huffnewNode->right = NULL;
    return huffnewNode;
}

// Function to create an internal node
Node *huffmancreatForInternalNode(int freQ) {
    return huffmancreateNode('\0', freQ);
}

// Function to swap two nodes in Priority Queue
void huff_swapNodes(Node **ab, Node **ba) {
    Node *t = *ab;
    *ab = *ba;
    *ba = t;
}

// Structure for Min Heap (Priority Queue)
typedef struct LessOrEqualchildren {
    int capacity;
    int size;
    Node **array;
} LessOrEqualchildren;

// Function to create a Min Heap
LessOrEqualchildren *createMinHeap(int capacity) {
    LessOrEqualchildren *lessOrEqualchildren = (LessOrEqualchildren *)malloc(sizeof(LessOrEqualchildren));
    lessOrEqualchildren->capacity = capacity;
    lessOrEqualchildren->size = 0;
    lessOrEqualchildren->array = (Node **)malloc(capacity * sizeof(Node *));
    return lessOrEqualchildren;
}

// Function to heapify a subtree at given index
void heapify(LessOrEqualchildren *lessOrEqualchildren, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    if (left < lessOrEqualchildren->size && lessOrEqualchildren->array[left]->freQ < lessOrEqualchildren->array[smallest]->freQ)
        smallest = left;
    if (right < lessOrEqualchildren->size && lessOrEqualchildren->array[right]->freQ < lessOrEqualchildren->array[smallest]->freQ)
        smallest = right;

    if (smallest != index) {
        huff_swapNodes(&lessOrEqualchildren->array[index], &lessOrEqualchildren->array[smallest]);
        heapify(lessOrEqualchildren, smallest);
    }
}

// Function to check if the heap has only one node
int isSizeOne(LessOrEqualchildren *lessOrEqualchildren) {
    return lessOrEqualchildren->size == 1;
}

// Function to insert a new node into Priority Queue
void insert(LessOrEqualchildren *lessOrEqualchildren, Node *node) {
    ++lessOrEqualchildren->size;
    int i = lessOrEqualchildren->size - 1;
    while (i && node->freQ < lessOrEqualchildren->array[(i - 1) / 2]->freQ) {
        lessOrEqualchildren->array[i] = lessOrEqualchildren->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    lessOrEqualchildren->array[i] = node;
}

// Function to extract the node with minimum frequency from Priority Queue
Node *extractMin(LessOrEqualchildren *lessOrEqualchildren) {
    Node *temp = lessOrEqualchildren->array[0];
    lessOrEqualchildren->array[0] = lessOrEqualchildren->array[lessOrEqualchildren->size - 1];
    --lessOrEqualchildren->size;
    heapify(lessOrEqualchildren, 0);
    return temp;
}

// Function to build the Huffman Tree
void buildHuffmanTree(LessOrEqualchildren *lessOrEqualchildren) {
    while (!isSizeOne(lessOrEqualchildren)) {
        Node *left = extractMin(lessOrEqualchildren);
        Node *right = extractMin(lessOrEqualchildren);

        Node *internalNode = huffmancreatForInternalNode(left->freQ + right->freQ);
        internalNode->left = left;
        internalNode->right = right;
        insert(lessOrEqualchildren, internalNode);
    }
}

// Function to print Huffman codes from Huffman Tree
void printHuffmanCode(Node *root, int *code, int top) {
    if (root->right) {
        code[top] = 0;
        printHuffmanCode(root->right, code, top + 1);
    }
    if (root->left) {
        code[top] = 1;
        printHuffmanCode(root->left, code, top + 1);
    }
    if (!root->left && !root->right) {
        if (root->theData == ' ') {
            printf("SPACE\t%d\t", root->freQ);
        } else {
            printf("%c\t%d\t", root->theData, root->freQ);
        }
        for (int i = 0; i < top; ++i)
            printf("%d", code[i]);
        printf("\n");
    }
}

// Function to compress input file using Huffman codes
void compressFile(FILE *huffInputFile, FILE *outputFile, int *code, int top) {
    int buffer = 0;
    int bitWrite = 0;
    fseek(huffInputFile, 0, SEEK_SET);
    char c;
    while (fread(&c, sizeof(char), 1, huffInputFile) > 0) {
        for (int i = 0; i < top; ++i) {
            buffer = (buffer << 1) | code[i];
            ++bitWrite;
            if (bitWrite == 8) {
                fwrite(&buffer, sizeof(char), 1, outputFile);
                buffer = 0;
                bitWrite = 0;
            }
        }
    }
    if (bitWrite > 0) {
        buffer = buffer << (8 - bitWrite);
        fwrite(&buffer, sizeof(char), 1, outputFile);
    }
}

int main(int argc, char *argv[]) {
    int debug = 0;
    char *inputFileName = "completeShakespeare.txt";
    char *outputFileName = "huffman.bin";
    int opt;

    while ((opt = getopt(argc, argv, "i:o:d")) != -1) {
        switch (opt) {
            case 'i':
                inputFileName = optarg;
                break;
            case 'o':
                outputFileName = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s -i inputFileName -o outputFileName -d\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    FILE *huffInputFile = fopen(inputFileName, "rb");
    if (!huffInputFile) {
        perror("Error opening inputFile");
        exit(EXIT_FAILURE);
    }

    FILE *outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        perror("Error opening outputFile");
        fclose(huffInputFile);
        return 1;
    }

    int freq[256] = {0};
    char ch;
    while (fread(&ch, sizeof(char), 1, huffInputFile) > 0) {
        freq[(unsigned char)ch]++;
    }

    fseek(huffInputFile, 0, SEEK_SET);
    int code[256];
    top = 0;
    compressFile(huffInputFile, outputFile, code, 0);

    fseek(outputFile, 0, SEEK_END);
    long compressedFileSize = ftell(outputFile);

    fseek(huffInputFile, 0, SEEK_END);
    long originalFileSize = ftell(huffInputFile);

    printf("%s TotalLength %ld\n", inputFileName, originalFileSize);

    LessOrEqualchildren *lessOrEqualchildren = createMinHeap(256);

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            Node *node = huffmancreateNode(i, freq[i]);
            insert(lessOrEqualchildren, node);
        }
    }

    buildHuffmanTree(lessOrEqualchildren);

    if (debug) {
        printf("Char, Frequency, Code\n");
        printHuffmanCode(lessOrEqualchildren->array[0], (int *)malloc(256 * sizeof(int)), 0);
    }

    free(lessOrEqualchildren->array);
    free(lessOrEqualchildren);
    fclose(huffInputFile);
    fclose(outputFile);

    return 0;
}
