#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#define _GNU_SOURCE
#define STOP_WORDS_SIZE 128
#define BUFFER_SIZE 1025
#define LINE_BUFFER_SIZE 81
#define CHAR_SIZE_TO_READ 1024

bool isInStopWords(char **str, char ***stopWords)
{
  for (int i = 0; i < STOP_WORDS_SIZE; ++i)
  {
    if ((*stopWords)[i] == NULL)
      break;
    if ((*stopWords)[i] == *str)
      return true;
  }

  return false;
}

int main(int argc, char *argv[])
{
  FILE *fptr;

  fptr = fopen("../stop_words.txt", "r");
  if (fptr == NULL)
  {
    printf("Could not open file :(");
    return -1;
  }

  void **data = calloc(8, sizeof(void *));

  char buffer[BUFFER_SIZE] = {0};
  fread(buffer, sizeof(char), CHAR_SIZE_TO_READ, fptr);
  fclose(fptr);

  data[0] = (char **)calloc(STOP_WORDS_SIZE, sizeof(char *));

  char wordBuffer[128] = {0};
  int j = 0;
  for (int i = 0, k = 0; i < BUFFER_SIZE; ++i)
  {
    if (buffer[i] == ',')
    {
      wordBuffer[k] = '\0';

      ((char **)data[0])[j] = strdup(wordBuffer);
      memset(wordBuffer, 0, sizeof(wordBuffer));
      k = 0, ++j;
    }
    else
      wordBuffer[k++] = buffer[i];
  }
  ((char **)data[0])[j] = wordBuffer + '\0';

  data[1] = (char *)calloc(LINE_BUFFER_SIZE, sizeof(char)); // data[1] is line (max 80 characters)
  data[2] = malloc(sizeof(int));                            // data[2] is index of the start_char of the word
  data[3] = malloc(sizeof(int));                            // data[3] is index on characters, i = 0
  data[4] = malloc(sizeof(bool));                           // data[4] is flag indicating if word was found
  data[5] = calloc(1024, sizeof(char));                     // data[5] is the word
  data[6] = calloc(1024, sizeof(char));                     // data[6] is the word,NNNN
  data[7] = malloc(sizeof(int));                            // data[7] is frequency

  FILE *secondaryMemory;
  secondaryMemory = fopen("word_freqs.txt", "w+");

  if (secondaryMemory == NULL)
  {
    printf("Could not create secondary memory");
    return -1;
  }

  FILE *inputFile;
  inputFile = fopen("../pride-and-prejudice.txt", "r");

  size_t len = 0;

  // Loop through the lines
  while (fgets(data[1], LINE_BUFFER_SIZE, inputFile) != NULL)
  {
    *(int *)data[2] = -1;
    *(int *)data[3] = 3;

    // Loop through the characters in the line
    for (int i = 3; i < LINE_BUFFER_SIZE; ++i)
    {
      char c = ((char *)data[1])[i];
      if (c == '\n')
        break;

      // If we have not found the start index of a word
      if (*(int *)data[2] == -1 && isalnum(c))
        *(int *)data[2] = *(int *)data[3];
      else if (!isalnum(c)) // This is the end of the word
      {
        *(bool *)data[4] = false;

        // Extract the word
        for (int j = *(int *)data[2], k = 0; j < *(int *)data[3]; ++j, ++k)
          ((char *)data[5])[k] = ((char *)data[1])[j];

        int length = *(int *)data[3] - *(int *)data[2];

        // Ignore words with a length less than 2 or if it is a stop word
        if (length >= 2 && !isInStopWords(data[5], data[0]))
        {
        }
      }

      (*(int *)data[3])++;
    }

    break;
  }

  fclose(inputFile);
  fclose(secondaryMemory);

  return 0;
}
