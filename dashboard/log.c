#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>

FILE * log_fp;

char *
get_time ()
{
  time_t rawtime;
  time (&rawtime);
  char * time_str = ctime (&rawtime);
  time_str[strlen(time_str)-1] = '\0';
  return time_str;
}

void
log_open (char * path)
{
  log_fp = fopen (path, "a");
  fprintf (log_fp, "%s: Log File '%s' Opened.  Blob v1.0\n", get_time (), path);
}

void
log_write (char * fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  fprintf (log_fp, "%s: ", get_time ());
  vfprintf (log_fp, fmt, args);
  fprintf (log_fp, "\n");
  va_end (args);
}
