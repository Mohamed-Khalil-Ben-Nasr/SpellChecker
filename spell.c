#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
    int N; // Number of words in the the dictionary
    int *index;
    char *words;
} dictionary;

/*function to get size of the file.*/
long int findSize(const char *file_name)
{
    struct stat st; /*declare stat variable*/

    /*get the size using stat()*/

    if (stat(file_name, &st) == 0)
        return (st.st_size);
    else
        return -1;
}

dictionary *loadDictionary(const char *indexName, const char *dictName)
{

    dictionary *d = (dictionary *)malloc(sizeof(dictionary));
    int index_fd = open(indexName, O_RDONLY);
    read(index_fd, &(d->N), sizeof(int)); // Read the number of words (N) why use&
    d->index = (int *)malloc((d->N + 1) * sizeof(int));
    read(index_fd, d->index, (d->N) * sizeof(int));
    close(index_fd);

    long int size = findSize(dictName);
    int words_fd = open(dictName, O_RDONLY);
    d->words = (char *)malloc(size + 1);
    read(words_fd, d->words, size);

    d->words[size] = '\0';

    for (int i = 0; i < size; i++)
    {
        if (d->words[i] == '\n')
        {
            d->words[i] = '\0';
        }
    }

    return d;
}

void freeDictionary(dictionary *d)
{
    free(d->words);
    free(d->index);
    free(d);
}

int searchDictionary(dictionary *d, const char *word)
{
    int start = 0;
    int end = d->N - 1;
    char curWord[64];
    while (start <= end)
    {
        int mid = (start + end) / 2;
        strcpy(curWord, &(d->words[d->index[mid]])); // Copy the word to curWord
        int comp = strcmp(word, curWord);
        if (comp == 0)
            return mid;
        else if (comp < 0)
            end = mid - 1;
        else
            start = mid + 1;
    }
    return -1;
}

void checkWords(dictionary *d, const char *fileName)
{
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
    {
        printf("Could not open data file.\n");
        return;
    }
    char line[256];
    char *word = NULL;
    const char *delims = " 0123456789!@#$&?%*+-/<>,.;:(){}[]\"\'\n\r\t";
    while (!feof(f))
    {
        fgets(line, 255, f);
        word = strtok(line, delims);
        while (word != NULL)
        {
            int n = 0;
            while (word[n] != '\0')
            {
                word[n] = tolower(word[n]);
                n++;
            }
            int loc = searchDictionary(d, word);
            if (loc == -1)
                printf("%s\n", word);
            word = strtok(NULL, delims);
        }
    }
    fclose(f);
}

int main()
{
    dictionary *d = loadDictionary("index.idx", "words.txt");
    if (d == NULL)
    {
        printf("Can not load dictionary.\n");
        return 1;
    }

    checkWords(d, "text.txt");

    freeDictionary(d);
    return 0;
}