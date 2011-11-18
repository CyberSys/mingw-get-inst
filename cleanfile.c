/**
 * cleanlog.c -- cleans text files with embedded control characters
 *    Currently only handles "CR" used for terminal updates:
 *      file-downloading  |=====                   |  15%^M
 *      file-downloading  |======                  |  17%^M
 *      ...
 *      file-downloading  |========================| 100%^M^J
 *    is transformed by retaining only the last "line", just
 *    as it would appear on a terminal, where each CR (^M)
 *    without a LF (^J) would cause the preceeding "line" to
 *    be overwritten.
 *
 * Copyright 2011 by Charles Wilson
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the COPYING file for full license information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#if defined(__WIN32__) && !defined(__CYGWIN__)
# include <io.h>
# include <conio.h>
#endif

static void usage (FILE * f, char *name);
static int  cleanfile (FILE * in, FILE * out);

static char *program_name;

int
main (int argc, char **argv)
{
  int rc;
  int ec = 0;
  int c;
  FILE * in = stdin;
  FILE * out = stdout;

  if ((program_name = strdup (argv[0])) == NULL)
    {
      fprintf (stderr, "%s: memory allocation error\n", argv[0]);
      exit (1);
    }

  opterr = 0;
  while ((c = getopt (argc, argv, "h")) != -1)
    switch (c)
      {
      case 'h':
        usage (stdout, program_name);
        ec = 0;
        goto finish;
      case '?':
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        ec = 2;
        goto finish;
      default:
        fprintf (stderr, "Internal error processing options\n");
        ec = 2;
        goto finish;
      }

  if (optind < argc - 2)
    {
      fprintf (stderr, "Too many arguments.\n");
      usage (stderr, program_name);
      ec = 2;
      goto finish;
    }
  if (optind < argc - 1)
    {
      if (strcmp (argv[optind], argv[optind+1]) == 0)
        {
          fprintf (stderr,
                   "Can't use same file %s as both input and output\n",
                   argv[optind]);
          ec = 1;
          goto finish;
        }
      out = fopen (argv[optind+1], "wb");
      if (out == NULL)
        {
          int serrno = errno;
          fprintf (stderr, "Can't open output file %s (%s)\n",
                   argv[optind+1], strerror(serrno));
          ec = 1;
          goto finish;
        }
    }
  if (optind < argc)
    {
      in = fopen (argv[optind], "rb");
      if (in == NULL)
        {
          int serrno = errno;
          fprintf (stderr, "Can't open input file %s (%s)\n",
                   argv[optind], strerror(serrno));
          ec = 1;
          goto finish;
        }
    }

  ec = cleanfile (in, out);

finish:
  free (program_name);
  if (out && out != stdout) fclose (out);
  if (in  && in  != stdin)  fclose (in);
  return (ec);
}

static void
usage (FILE * f, char *name)
{
  fprintf (f, "%s [-h] [inputfile [outputfile]]\n", name);
}

#define MAX_LINE 1024
static int
cleanfile (FILE * in, FILE * out)
{
  char linebuf[MAX_LINE];
  int pos = 0;
  int gotCR = 0;
  int ec = 0;
  int c;
  while (1)
    {
      c = getc (in);
      if (c == EOF)
        {
          if (pos > 0)
            {
              if (fwrite (linebuf, sizeof (char), pos, out) != pos)
                {
                  fprintf (stderr, "Error writing output\n");
                  ec = 3;
                }
            }
          break;
        }
      if (c == '\r')
        {
          gotCR = 1;
          linebuf[pos++] = c;
        }
      else if (c == '\n')
        {
          linebuf[pos++] = c;
          if (fwrite (linebuf, sizeof (char), pos, out) != pos)
            {
              fprintf (stderr, "Error writing output\n");
              ec = 3;
            }
          pos = 0;
          gotCR = 0;
        }
      else
        {
          if (gotCR)
            {
              pos = 0;
              gotCR = 0;
            }
          linebuf[pos++] = c;
        }
      if (pos == MAX_LINE)
        {
          /* line too long; just write what we have */
          if (fwrite (linebuf, sizeof (char), pos, out) != pos)
            {
              fprintf (stderr, "Error writing output\n");
              ec = 4;
            }
          pos = 0;
          gotCR = 0;
          fprintf (stderr, "Line too long...continuing\n");
        }
    }
    return ec;
}

