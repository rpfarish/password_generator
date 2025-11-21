#include <openssl/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_rand_range(int min, int max) {
  unsigned int rand_val;

  if (RAND_bytes((unsigned char *)&rand_val, sizeof(rand_val)) != 1) {
    fprintf(stderr, "RAND_bytes failed\n");
    exit(1);
  }

  int range = max - min + 1;
  unsigned int limit = UINT_MAX - (UINT_MAX % range);

  while (rand_val >= limit) {
    RAND_bytes((unsigned char *)&rand_val, sizeof(rand_val));
  }

  return min + (rand_val % range);
}

bool is_in_character_range(char c, bool lowercase, bool uppercase, bool numbers,
                           bool symbols) {

  if (symbols && 33 <= c && c <= 47)
    return true;

  if (numbers && '0' <= c && c <= '9')
    return true;

  if (symbols && 58 <= c && c <= 64)
    return true;

  if (uppercase && 'A' <= c && c <= 'Z')
    return true;

  if (symbols && 91 <= c && c <= 95)
    return true;

  if (lowercase && 'a' <= c && c <= 'z')
    return true;

  if (symbols && 123 <= c && c <= 126)
    return true;

  return false;
}

int main(int argc, char *argv[]) {
  int password_length = 15;
  bool lowercase = true;
  bool uppercase = true;
  bool numbers = true;
  bool symbols = true;
  char *exclude = NULL;
  size_t exclude_len = 0;

  int i = 1;
  while (i < argc) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'l':
        password_length = atoi(argv[i + 1]);
        if (password_length < 1) {
          printf("Please enter a number greater than 0\n");
          return 0;
        }
        i++;
        break;
      case 'e':
        exclude = argv[i + 1];
        exclude_len = strlen(exclude);
        i++;
        break;
      case 's':
        symbols = false;
        break;
      case 'u':
        lowercase = false;
        break;
      case 'U':
        uppercase = false;
        break;
      case 'n':
        numbers = false;
        break;
      default:
        printf("Invalid option %s\n", argv[i]);
        return 1;
      }
    }
    i++;
  }

  if (!lowercase && !uppercase && !numbers && !symbols)
    return 0;

  char *password = (char *)malloc(password_length + 1);
  if (password == NULL) {
    printf("Password memory allocation failed!");
    return 1;
  }

  bool skip_char;
  int next_char;
  bool is_in_range;
  i = 0;
  while (i < password_length) {
    next_char = get_rand_range(33, 126);
    is_in_range = is_in_character_range(next_char, lowercase, uppercase,
                                        numbers, symbols);

    if (!is_in_range)
      continue;

    skip_char = false;
    if (exclude != NULL)
      for (size_t j = 0; j < exclude_len; j++)
        if (next_char == exclude[j]) {
          skip_char = true;
          break;
        }

    if (skip_char)
      continue;

    password[i] = next_char;
    i++;
  }

  password[password_length] = '\0';
  printf("%s\n", password);

  free(password);
  return 0;
}
