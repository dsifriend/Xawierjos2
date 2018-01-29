#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* We make use of POSIX error codes */
#include <errno.h>

/* Alphabet key used for sorting */
const char alphabet[] = "aābgdeēzhθiījklmnoōprstuūwyȳφx";
const size_t alpha_size = sizeof(alphabet)/sizeof(alphabet[0]);

/* Character delimiter for entry sorting function */
const int DELIM = ' ';
/* Variables for number of entries and their length in dictionary.
 * Must be global to access from comparator function. */
size_t entry_size;
size_t vocab_size;

/* Function to compare words in list for lexicographical ordering. */
int lex_comp(const void *w0, const void *w1);
/* Function to get max size of dictionary entries. */
size_t max_entry_size(FILE *entries);
/* Function to get amount of entries in dictionary.*/
size_t max_vocab_size(FILE *entries);
/* Function to import entries from file into array. */
void import_entries(FILE *entries, char *list,
        size_t vocab_size, size_t entry_size);
/* Function to export entries from array into file. */
void export_entries(FILE *entries, const char *list,
        size_t vocab_size, size_t entry_size);

int main(int argc, char *argv[])
{
    /* Open up first argument as unsorted entries, try ./unsorted_entries.txt
     * if not given, throw error otherwise. */
    FILE *unsrt;
    unsrt = fopen(argv[1], "r");
    if (unsrt == NULL) {
        unsrt = fopen("unsorted_entries.txt", "r");
        perror("No arguments given, trying \"./unsorted_entries.txt\".\n");
    }
    if (unsrt == NULL) {
        perror("Could not find \"./unsorted_entries.txt\". Exiting now...\n");
        return EBADF;
    }
    /* Creates file to sort dictionary entries into. */
    FILE *sortd;
    sortd = fopen("sorted_entries.txt", "w+");
    if (sortd == NULL) {
        perror("Failed to create file for output. Exiting now...\n");
        return EBADF;
    }

    /* Get size and amount of dictionary entries for copying into the heap. */
    entry_size = max_entry_size(unsrt);
    vocab_size = max_vocab_size(unsrt);

    /* Allocate memory for dictionary entries in the heap. */
    char *entry_list;
    entry_list = calloc(vocab_size, entry_size);
    /* Copy entries into list before sorting. */
    import_entries(unsrt, entry_list, vocab_size, entry_size);

    /* Sort the entries list in place, then write to file */
    qsort(entry_list, vocab_size, entry_size, lex_comp);
    export_entries(sortd, entry_list, vocab_size, entry_size);

    /* Close files, free memory */
    fclose(unsrt); fclose(sortd);
    free(entry_list);

    return 0;
}

size_t max_entry_size(FILE *entries)
{
    size_t tmp_size = 0;
    size_t tmp_max = 0;

    int tmp_char;
    do {
        tmp_char = fgetc(entries);
        ++tmp_size;
        if (tmp_char == '\n' || tmp_char == EOF) {
            tmp_max = (tmp_size > tmp_max) ? tmp_size : tmp_max;
            tmp_size = 0;
        }
    } while (tmp_char != EOF);
    fseek(entries, 0, SEEK_SET);

    /* Must add 1 to account for newlines or EOF */
    return ++tmp_max;
}

size_t max_vocab_size(FILE *entries)
{
    size_t tmp_size = 0;
    size_t tmp_max = 0;

    int tmp_char;
    do {
        tmp_char = fgetc(entries);
        if (tmp_char == '\n' || tmp_char == EOF)
            ++tmp_size;
    } while (tmp_char != EOF);
    tmp_max = tmp_size;
    fseek(entries, 0, SEEK_SET);

    return tmp_max;
}

int lex_comp(const void *w0, const void *w1)
{
    //TODO The commented block keeps the algorithm from segfaulting with newlines. Figure out how to get rid of it.

    char *tmp_w0 = calloc(sizeof(char), entry_size);
    char *tmp_w1 = calloc(sizeof(char), entry_size);
    strcpy(tmp_w0, w0);
    strcpy(tmp_w1, w1);

    int tmp_char0 = -1;
    int tmp_char1 = -1;
    for (size_t pair = 0; tmp_w0[pair] != DELIM || tmp_w1[pair] != DELIM; ++pair) {
        /*if (tmp_w0[pair] == '\n') {
            printf("Newline,\n");
            break;
        }*/
        for (size_t i = 0; i < alpha_size; ++i) {
            if (tmp_w0[pair] == alphabet[i])
                tmp_char0 = i;
            if (tmp_w1[pair] == alphabet[i])
                tmp_char1 = i;
        }
        if (tmp_char0 != tmp_char1)
                goto return_val;
    }

    return_val: {
        if (tmp_char0 < tmp_char1)
            return 1;
        if (tmp_char0 > tmp_char1)
            return -1;
    }

    perror("Comparing newlines, maybe");
    return 0;
}


void import_entries(FILE *entries, char *list,
        size_t vocab_size, size_t entry_size)
{
    for (size_t i = 0; i < vocab_size; ++i) {
        fgets(&list[i*entry_size], (int) entry_size, entries);
    }
}

void export_entries(FILE *entries, const char *list,
        size_t vocab_size, size_t entry_size)
{
    for (size_t i = 0; i < vocab_size; ++i) {
        fputs(&list[i*entry_size], entries);
    }
}
