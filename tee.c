/*******************************************************************************
* tee.c
*	Splits standard input stream to standard output stream and
*	one or more output files.
*
* Notice
*	Copyright ©1985-2006 by David R Tribble, all rights reserved.
*
*       [csw: Website http://david.tribble.com/programs.html says
*             "Distribution and use is free and unlimited unless
*             otherwise noted."  As there is no contrary statement
*             concerning tee, I'm assuming this usage release applies.
*             No changes, other than this comment, have been made to
*             the source code. Recompiled using MinGW:
*                gcc -static -static-libgcc -o tee.exe tee.c
*                strip tee.exe
*       ]
*
* History
*	1.4 2006-06-26 drt.
*	Added command line options '-a', '-n'.
*/


/* Identification */

static const char	PROG[] =	"tee";
static const char	VERS[] =	"1.4";
static const char	DATE[] =	"2006-06-26";
static const char	AUTH[] =	"David R Tribble";

static const char	REV[] =
    "@(#)drt/src/cmd/tee.c $Revision$$Date$";
static const char	BUILT[] =
    "@(#)" "Built: " __DATE__ " " __TIME__;

static const char	COPYRIGHT[] =
    "@(#)Copyright ©1985-2006 by David R Tribble, all rights reserved.";


/* System includes */

#include <iso646.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>

#if _WIN32
 #include <io.h>
#endif


/* Local manifest constants */

typedef int		bool;

#define false		0
#define true		1

#define MAXFILE		300		/* Maximum output files		*/


/* Local variables */

static bool	opt_nlflush =	false;	/* Flush after newlines		*/
static bool	opt_append =	false;	/* Append to output files	*/


/*******************************************************************************
* tee()
*	Split stdin input stream into stdout stream and one or more
*	output files.
*
* History
*	1.0 1985-04-15 drt.
*	First revision.
*
*	1.1 1987-10-15 drt.
*	File modes are now binary.
*
*	1.4 2006-06-26 drt.
*	Added 'opt_nlflush' and 'opt_append' flags.
*/

void tee(int numfiles, char *filename[])
{
    const char *	mode;		/* Output file mode		*/
    FILE *		fp[MAXFILE];	/* Output file pointers		*/
    int			i;		/* File counter			*/

    /* Open the output files */
    mode = "wb";
    if (opt_append)
        mode = "ab";

    for (i = 0;  i < numfiles  and  i < MAXFILE;  ++i)
    {
        fp[i] = fopen(filename[i], mode);
        if (fp[i] == NULL)
            fprintf(stderr, "%s: Can't write: %s\n",
                PROG, filename[i]);
    }

    /* Split the input to several output files */
    for (;;)
    {
        int	c;		/* Input/output char			*/

        c = getchar();
        if (c == EOF)
            break;

        /* Send char to output files */
        for (i = 0;  i < numfiles  and  i < MAXFILE;  ++i)
            if (fp[i] != NULL)
                putc(c, fp[i]);

        /* Send char to stdout */
        putchar(c);

        /* Handle flushing after newlines */
        if (c == '\n'  and  opt_nlflush)
        {
            /* Flush output files */
            for (i = 0;  i < numfiles  and  i < MAXFILE;  ++i)
                if (fp[i] != NULL)
                    fflush(fp[i]);

            /* Flush stdout */
            fflush(stdout);
        }
    }

    /* Close the files */
    for (i = 0;  i < numfiles  and  i < MAXFILE;  ++i)
    {
        if (fp[i] == NULL)
            fclose(fp[i]);
    }

    fflush(stdout);
}


/*******************************************************************************
* main()
*
* History
*	1.0 1985-04-15 drt.
*	First revision.
*
*	1.1 1987-10-15 drt.
*	File modes are now binary.
*
*	1.2 1988-01-11 drt.
*	Uses setvbuf() for stdin.
*
*	1.4 2006-06-26 drt.
*	Added command line options '-a', '-n'.
*/

int main(int argc, char *argv[])
{
    char	invbuf[10*1024];	/* Stdin buffer			*/

    /* Parser command line options */
    while (argc > 1  and  argv[1][0] == '-')
    {
        if (strcmp(argv[1], "-a") == 0)
            opt_append = true;
        else if (strcmp(argv[1], "-n") == 0)
            opt_nlflush = true;
        else
            goto usage;

        argc--, argv++;
    }

    /* Check command line args */
    if (argc < 2  or  isatty(fileno(stdin)))
        goto usage;

#ifdef O_BINARY
    /* Open stdin in binary mode */
    if (setmode(fileno(stdin), O_BINARY) == -1)
    {
        fprintf(stderr, "%s: Can't set mode on standard input\n",
            PROG);
        return EXIT_FAILURE;
    }
#endif
    setvbuf(stdin, invbuf, _IOFBF, sizeof invbuf);

#ifdef O_BINARY
    /* Open stdout in binary mode */
    if (setmode(fileno(stdout), O_BINARY) == -1)
    {
        fprintf(stderr, "%s: Can't set mode on standard output\n",
            PROG);
        return EXIT_FAILURE;
    }
#endif

    /* Split output into one or more file streams */
    tee(argc-1, argv+1);
    return EXIT_SUCCESS;

usage:
    /* Display a usage message */
    printf("[%s %s, %s %s]\n\n",
        PROG, VERS, AUTH, DATE);
    printf("Split output into two or more streams.\n\n");

    printf("usage:  %s [-option...] file... <input\n\n", PROG);
    printf("Options:\n");
    printf("    -a      Append output to the file(s).\n");
    printf("    -n      Flush output after newlines.\n");

    return EXIT_FAILURE;
}

/* End tee.c */
