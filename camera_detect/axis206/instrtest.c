#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char str_1[] = "this is a test string";
char str_2[] = "is a";

char *
str_token (char * str, char * instr, int strlen, int instrlen)
{
  int i, j;
  for (i = 0; i < strlen; i++) {
    for (j = 0; j < instrlen; j++) {
      printf ("str[i+j]: %c\tinstr[j]:%c\n", str[i+j], instr[j]);
      if (str[i+j] != instr[j])
        break;
      if (j == instrlen-1)
        return str + i;
    }
  }
  return NULL;
}

void
main (void)
{
  char * result = str_token (str_1, str_2, 15, 4);
  printf ("str_1: %s\nstr_2: %s\n res: %s\n", str_1, str_2, result);
}
