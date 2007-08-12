/* print.c - displays checksum listing
   
   Copyright (C) 2000 Bryan Call <bc@fodder.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <libgen.h>

#include "cksfv.h"
#include "config.h"

#define WEBSITE "http://www.iki.fi/shd/foss/cksfv/"

void pnsfv_head()
{
    time_t clock;
    struct tm *timeinfo;
    struct tm ti;

    clock = time(NULL);
    timeinfo = localtime(&clock);
    if (timeinfo == NULL) {
	memset(&ti, 0, sizeof(ti));
	ti.tm_year = 70;
	timeinfo = &ti;
    }
    printf("; Generated by cksfv v%s on ", VERSION);
    printf("%02d-%02d-%02d at %02d:%02d.%02d\n", timeinfo->tm_year + 1900,
	   timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
	   timeinfo->tm_min, timeinfo->tm_sec);
    printf("; Project web site: %s\n", WEBSITE);
}

void pfileinfo(char **argv)
{
    char *fn;
    struct stat sb;
    struct tm *timeinfo;
    char *tmpname;
    char *fname;

    printf(";\n");

    while (*argv) {
	fn = *argv++;
	if (!(stat(fn, &sb))) {
	    if (!S_ISDIR(sb.st_mode)) {

		tmpname = NULL;
		if (use_basename) {
		    if ((tmpname = strdup(fn)) == NULL) {
			if (!TOTALLY_QUIET)
			    fprintf(stderr, "out of memory\n");
			exit(-1);
		    }
		    fname = basename(tmpname);
		} else {
		    fname = fn;
		}

		timeinfo = localtime(&sb.st_mtime);
		printf(";%13lu  %02d:%02d.%02d %02d-%02d-%02d %s\n",
		       (unsigned long) sb.st_size, timeinfo->tm_hour,
		       timeinfo->tm_min, timeinfo->tm_sec,
		       timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
		       timeinfo->tm_mday, fname);
		if (use_basename)
		    free(tmpname);
	    }
	}
    }
}

void pcrc(char *fn, uint32_t val)
{
    printf("%s %.8X\n", fn, val);
}

void prsfv_head(char *fn)
{
    char head[82], *p;
    int len;

    /* this is b0rken. assumes terminal and filename widths. however, this
       won't cause the program to fail in the real purpose */
    snprintf(head, 80, "--( Verifying: %s", fn);
    len = strlen(head);
    if (len < 76) {
	p = head + len;
	*p++ = ' ';
	*p++ = ')';

	for (p = head + len + 2, len = 78 - len; len--; p++)
	    *p = '-';
    } else {
	p = head + 76;
	*p++ = ' ';
	*p++ = ')';
	*p++ = '-';
	*p++ = '-';
    }
    head[80] = '\n';
    head[81] = '\0';
    fprintf(stderr, "%s", head);
}

void pusage()
{
    printf
	("cksfv v%s:\tBryan Call <bc@fodder.org>, and later modified by\n\t\tHeikki Orsila <heikki.orsila@iki.fi>.\n",
	 VERSION);
    printf("\tweb:\t%s\n\n", WEBSITE);
    printf("usage: cksfv [-bciq] [-C directory] [-f file.sfv] ");
    printf("[file ...]\nsupported options:\n\n");
    printf
	(" -b\tprint only the basename when creating an sfv (no directory component)\n");
    printf
	(" -c\tuse stdout for printing progress and final resolution (useful for\n");
    printf("\texternal tools analysing cksfv output)\n");
    printf(" -C\tchange to directory for processing\n");
    printf(" -f\t.sfv file to verify\n");
    printf(" -i\tignore case on filenames\n");
    printf(" -L\tfollow symlinks in recursive mode\n");
    printf(" -q\tquiet, only prints error messages\n");
    printf(" -r\trecursively check .sfv files in subdirectories\n");
    printf(" -s\treplace backslashes with slashes on filenames\n");
    printf(" -v\tverbose, by default this option is on\n");
    exit(1);
}
