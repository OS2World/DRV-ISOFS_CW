/* mntaefs.c -- ISOFS mount program.
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
#include <process.h>
#include <string.h>
#include <ctype.h>

#include <os2.h>

#include "getopt.h"

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
     (c) Chris Wohlgemuth 2000-2001\n\
     http://www.geocities.com/SiliconValley/Sector/5785/index.html\n\n");

      printf("\
Usage: %s [OPTION]... DRIVE-LETTER: ISO-PATH\n\
Mount the ISO file stored in ISO-PATH onto DRIVE-LETTER.\n\
\n\
      --help               display this help and exit\n\
      --version            output version information and exit\n\
      --offset (-o) <nnn>  offset of session on CD\
      --jcharset <CP>      Translation codepage for unicode names.\
                           Avaiable codepages:\
                                           cp437 cp737 cp775\n\
                                           cp850 cp852 cp855\n\
                                           cp857 cp860 cp861\n\
                                           cp862 cp863 cp864\n\
                                           cp865 cp866 cp869\n\
                                           cp874 iso8859-1 iso8859-2\n\
                                           iso8859-3 iso8859-4 iso8859-5\n\
                                           iso8859-6 iso8859-7 iso8859-8\n\
                                           iso8859-9 iso8859-14 iso8859-15\n\
                                           koi8-r\n\
Examples:\n\
  Mount the ISO image file in `c:\\isoimage.raw' onto drive X:\n\
\n\
  %s x: c:\\isoimage.raw\n\
",
         pszProgramName, pszProgramName);
   }
   exit(status);
}


int main(int argc, char * * argv)
{

   int iOffset=0;
   int c;
   AEFS_ATTACH attachparms;
   APIRET rc;
   char  * pszDrive, * pszBasePath;
   char  * pszCharSet="";

   struct option const options[] = {
      { "help", no_argument, 0, 1 },
      { "version", no_argument, 0, 2 },
      { "jcharset", required_argument, 0, 'j' },
      { "offset", required_argument, 0, 'o' },
      { 0, 0, 0, 0 } 
   };      

   /* Parse the arguments. */
   
   pszProgramName = argv[0];

   while ((c = getopt_long(argc, argv, "o:j:", options, 0)) != EOF) {
      switch (c) {
         case 0:
            break;

         case 1: /* --help */
            printUsage(0);
            break;

         case 2: /* --version */
            printf("mntisofs - %s\n", AEFS_VERSION);
            exit(0);
            break;

      case 'j': /* --jcharset */
        pszCharSet = optarg;
        break;

         case 'o': /* --offset */
           iOffset=atoi(optarg);
            break;

         default:
            printUsage(1);
      }
   }

   if (optind != argc - 2) {
      fprintf(stderr, "%s: missing or too many parameters\n", pszProgramName);
      printUsage(1);
   }

   pszDrive = argv[optind++];
   pszBasePath = argv[optind++];

   memset(&attachparms, 0, sizeof(attachparms));

   /* Drive okay? */
   if ((strlen(pszDrive) != 2) ||
       (!isalpha((int) pszDrive[0])) ||
       (pszDrive[1] != ':'))
   {
      fprintf(stderr, "%s: drive specification is incorrect\n",
         pszProgramName);
      return 1;
   }

   /* Does the base path fit? */
   if (strlen(pszBasePath) >= sizeof(attachparms.szBasePath)) {
      fprintf(stderr, "%s: base path name is too long\n",
         pszProgramName);
      return 1;
   }
   strcpy(attachparms.szBasePath, pszBasePath);
   if(pszCharSet)
     strcpy(attachparms.szCharSet, pszCharSet);

   /* Expand the given base path.  (The daemon does not accept
      relative path names). */
   if (_abspath(attachparms.szBasePath, attachparms.szBasePath,
      sizeof(attachparms.szBasePath)))
   {
      fprintf(stderr, "%s: cannot expand path\n", pszProgramName);
      return 1;
   }
  
   attachparms.iOffset=iOffset;
  
   /* Send the attachment request to the FSD. */
   rc = DosFSAttach(
      (PSZ) pszDrive,
      (PSZ) AEFS_IFS_NAME,
      &attachparms,
      sizeof(attachparms),
      FS_ATTACH);
   if (rc) {

     fprintf(stderr, "%s: error mounting ISOFS volume, rc = %ld\n\n",
             pszProgramName, rc);
     switch(rc)
       {
       case ERROR_ISOFS_FILEOPEN:
         fprintf(stderr, "Can't open ISO imagefile. Check the name and path.\n");
         break;
       case ERROR_ISOFS_INVALIDOFFSET:
         fprintf(stderr, "Invalid offset.\n");
         break;
       case ERROR_ISOFS_WRONGJOLIETUCS:
       case ERROR_ISOFS_NOJOLIETSVD:
         fprintf(stderr, "No Joliet names or unknown Joliet format.\n");
         break;
       case ERROR_STUBFSD_DAEMON_NOT_RUNNING:
         fprintf(stderr, "The daemon is not running.\n");
         break;
       default:
         fprintf(stderr, "Make sure the drive letter isn't in use yet.\n");
         break;
       }
     return rc;
   }
   
   return 0;
}
