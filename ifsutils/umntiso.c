/* umntaefs.c -- ISOFS unmount program.
   Copyright (C) 1999 Eelco Dolstra (edolstra@students.cs.uu.nl).

   Modifications for ISOFS:
   Copyright (C) 2000 Chris Wohlgemuth (chris.wohlgemuth@cityweb.de)
   http://www.geocities.com/SiliconValley/Sector/5785/index.html

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <os2.h>

#include "getopt.h"

#include "types.h"
#include "aefsdint.h"


char * pszProgramName;


static void printUsage(int status)
{
   if (status)
      fprintf(stderr,
         "\nTry `%s --help' for more information.\n",
         pszProgramName);
   else {
     printf("\
This program is licensed according to the GPL.\n\
See file COPYING for further information.\n\
\n\
     (c) Eelco Dolstra\n\
     (c) Chris Wohlgemuth 2000\n\
     http://www.geocities.com/SiliconValley/Sector/5785/index.html\n\n");

      printf("\
Usage: %s [OPTION]... DRIVE-LETTER:\n\
Unmount the ISOFS volume from the specified DRIVE-LETTER.\n\
\n\
      --help         display this help and exit\n\
      --version      output version information and exit\n\
  -f, --force        unmount even when the volume cannot be\n\
                      flushed to disk\n\
\n\
Examples:\n\
  Unmount drive X:\n\n\
  %s x:\n\
",
         pszProgramName, pszProgramName);
   }
   exit(status);
}


int main(int argc, char * * argv)
{
   Bool fForceUnmount = FALSE;
   char * pszDrive;
   int c;
   APIRET rc;
   AEFS_DETACH detachparms;

   struct option const options[] = {
      { "help", no_argument, 0, 1 },
      { "version", no_argument, 0, 2 },
      { "force", no_argument, 0, 'f' },
      { 0, 0, 0, 0 } 
   };

   /* Parse the arguments. */
   
   pszProgramName = argv[0];

   while ((c = getopt_long(argc, argv, "f", options, 0)) != EOF) {
      switch (c) {
         case 0:
            break;

         case 1: /* --help */
            printUsage(0);
            break;

         case 2: /* --version */
            printf("umntiso - %s\n", AEFS_VERSION);
            exit(0);
            break;

         case 'f': /* --force */
            fForceUnmount = TRUE;
            break;

         default:
            printUsage(1);
      }
   }

   if (optind != argc - 1) {
      fprintf(stderr, "%s: missing or too many parameters\n", pszProgramName);
      printUsage(1);
   }

   pszDrive = argv[optind++];

   /* Drive okay? */
   if ((strlen(pszDrive) != 2) ||
       (!isalpha((int) pszDrive[0])) ||
       (pszDrive[1] != ':'))
   {
      fprintf(stderr, "%s: drive specification is incorrect\n",
         pszProgramName);
      return 1;
   }

   memset(&detachparms, 0, sizeof(detachparms));
   if (fForceUnmount) detachparms.flFlags |= DP_FORCE;
   
   /* Unmount the volume attached to the specified drive. */
   rc = DosFSAttach(
      (PSZ) pszDrive,
      (PSZ) AEFS_IFS_NAME,
      &detachparms,
      sizeof(detachparms),
      FS_DETACH);
   if (rc) {
      fprintf(stderr, "%s: error unmounting ISOFS volume, rc = %ld\n",
         pszProgramName, rc);
      return 1;
   }

   return 0;
}
