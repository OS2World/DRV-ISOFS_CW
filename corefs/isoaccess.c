/* isoaccess.c -- Routines for iso file handling.
   Copyright (C) 1999 Eelco Dolstra (edolstra@students.cs.uu.nl).

   Copyright (C) 2000 Chris Wohlgemuth (chris.wohlgemuth@cityweb.de)
   http://www.geocities.com/SiliconValley/Sector/5785/index.html

   Routines for iso9660 directory parsing: 
   
   Originally written by Eric Youngdale (1993).
   Copyright 1993 Yggdrasil Computing, Incorporated


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
#include <ctype.h>
#include <string.h>
#include <io.h>

#include <iconv.h>
#include <errno.h>
#include "aefsdmn.h"
#include "nls.h"

int
isonum_721 (p)
	char	*p;
{
	return ((p[0] & 0xff)
		| ((p[1] & 0xff) << 8));
}

int
isonum_723 (p)
	char * p;
{
#if 0
	if (p[0] != p[3] || p[1] != p[2]) {
#ifdef	USE_LIBSCHILY
		comerrno(EX_BAD, "invalid format 7.2.3 number\n");
#else
		fprintf (stderr, "invalid format 7.2.3 number\n");
		exit (1);
#endif
	}
#endif
	return (isonum_721 (p));
}

int
isonum_731 (p)
	char	*p;
{
	return ((p[0] & 0xff)
		| ((p[1] & 0xff) << 8)
		| ((p[2] & 0xff) << 16)
		| ((p[3] & 0xff) << 24));
}


int
isonum_733 (p)
	unsigned char	*p;
{
	return (isonum_731 ((char *)p));
}

IsoDirEntry*
getDirEntries(rootname, extent, len, pVolData, iOnlyDirs)
	char	*rootname;
	int	extent;
	int	len;
    VolData  * pVolData;
    int iOnlyDirs;
{
  char testname[256];

  int i;
  struct iso_directory_record * idr;

  unsigned char buffer[2048];
  struct stat fstat_buf;
  char name_buf[256];
  char xname[256];

  IsoDirEntry* pIsoDirEntry=NULL;
  IsoDirEntry* pIsoDirEntryTemp=NULL;

  unsigned char	uh,
    ul,
    uc,
    *up;

  memset(buffer,0, sizeof(buffer));

  while(len > 0 )
    {
      lseek(fileno(pVolData->isoFile), (extent - sector_offset) << 11, 0);
      read(fileno(pVolData->isoFile), buffer, sizeof(buffer));
      len -= sizeof(buffer);
      extent++;
      i = 0;
      while(1==1){
        idr = (struct iso_directory_record *) &buffer[i];
        if(idr->length[0] == 0) break;
        memset(&fstat_buf, 0, sizeof(fstat_buf));
        name_buf[0] = xname[0] = 0;
        fstat_buf.st_size = isonum_733((unsigned char *)idr->size);
        if( idr->flags[0] & 2)
          fstat_buf.st_mode |= S_IFDIR;
        else
          fstat_buf.st_mode |= S_IFREG;	
        if(idr->name_len[0] == 1 && idr->name[0] == 0)
          strcpy(name_buf, ".");
        else if(idr->name_len[0] == 1 && idr->name[0] == 1)
          strcpy(name_buf, "..");
        else {
          switch(pVolData->ucs_level)
            {
            case 3:
            case 2:
            case 1:
              /*
               * Unicode name.  Convert as best we can.
               */
              {
                int j;

                if(pVolData->nls) {
                  /* We use a translation table */
                  for(j=0; j < idr->name_len[0] / 2; j++)
                    {
                      uh = idr->name[j*2];	/*    hibyte...	     */
                      ul = idr->name[j*2+1];	/* ...lobyte	     */
                      up = pVolData->nls->page_uni2charset[uh];	/* convert backward:  page...	     */
                      if (up == NULL)
                        uc = '\0';	/* ...wrong unicode page     */
                      else
                        uc = up[ul];	/* backconverted character   */
                      name_buf[j] =uc;
                    }
                  name_buf[idr->name_len[0]/2] = '\0';

                }
                else {
                  /* Use poor mans translation */
                  for(j=0; j < idr->name_len[0] / 2; j++)
                    {
                      name_buf[j] = idr->name[j*2+1];
                    }
                  name_buf[idr->name_len[0]/2] = '\0';
                }
              }
              break;
            case 0:
              /*
               * Normal non-Unicode name.
               */
              strncpy(name_buf, idr->name, idr->name_len[0]);
              name_buf[idr->name_len[0]] = 0;
              break;
            default:
              /*
               * Don't know how to do these yet.  Maybe they are the same
               * as one of the above.
               */
              logMsg(L_FATAL, "getDirEntries(): Found strange ucs_level %d, don't know what to do! Bye...",
                     pVolData->ucs_level);
              exit(1);
            }
        };
                        
        /* Build name */
        strcpy(testname, name_buf);
        
        /*        if(!iOnlyDirs || (S_ISDIR(fstat_buf.st_mode) && iOnlyDirs)) {
         */
        if(!pIsoDirEntry) {
            pIsoDirEntry = calloc(sizeof(IsoDirEntry), sizeof(BYTE));
            if (!pIsoDirEntry) {
              logMsg(L_EVIL, "getDirEntries():  ERROR_NOT_ENOUGH_MEMORY;");
              return NULL;
            }
#if 0
            else
              logMsg(L_DBG, "calloc: %x", pIsoDirEntry);
#endif
            pIsoDirEntryTemp = pIsoDirEntry;
          }
          else {
            pIsoDirEntryTemp->pNext = calloc(sizeof(IsoDirEntry), sizeof(BYTE));
            if (!pIsoDirEntryTemp->pNext) {
              logMsg(L_EVIL, "getDirEntries():  ERROR_NOT_ENOUGH_MEMORY;");
              return pIsoDirEntry;
            }
#if 0
            else
              logMsg(L_DBG, "calloc: %x", pIsoDirEntryTemp->pNext);
#endif
            pIsoDirEntryTemp = pIsoDirEntryTemp->pNext;
          }
        /* }*/
        memcpy(pIsoDirEntryTemp->date_buf, idr->date, 9);
        strncpy(pIsoDirEntryTemp->chrFullPath,testname,sizeof(pIsoDirEntryTemp->chrFullPath));
        strncpy(pIsoDirEntryTemp->chrName,name_buf,sizeof(pIsoDirEntryTemp->chrName));

        pIsoDirEntryTemp->iParentExtent=extent-1;
        pIsoDirEntryTemp->iExtent=isonum_733((unsigned char *)idr->extent);
        pIsoDirEntryTemp->iSize=isonum_733((unsigned char *)idr->size);
        memcpy(&pIsoDirEntryTemp->fstat_buf, &fstat_buf  ,sizeof(fstat_buf));
        pIsoDirEntryTemp->flFlags=0;
        pIsoDirEntryTemp->flFlags|=FILE_READONLY;
        if(S_ISDIR(pIsoDirEntryTemp->fstat_buf.st_mode))
          pIsoDirEntryTemp->flFlags|=FILE_DIRECTORY;
        i += buffer[i];
        if (i > 2048 - sizeof(struct iso_directory_record)) break;
      }
    }
  return pIsoDirEntry;
}

void
isoFreeDirEntries(  IsoDirEntry* pIsoDirEntry) {

  IsoDirEntry* pTemp;/*,* pTemp2;*/
  
  if(!pIsoDirEntry)
    return;

#if 0
  pTemp2=pIsoDirEntry;

  do {
    pTemp=pTemp2->pNext;
    logMsg(L_DBG, "%x\n", pTemp2);
    free(pTemp2);
    logMsg(L_DBG, " freed...\n");
    pTemp2=pTemp;
  }  while(pTemp);
  logMsg(L_DBG, "Done\n");
#endif
  do {
    pTemp=pIsoDirEntry->pNext;
    free(pIsoDirEntry);
    pIsoDirEntry=pTemp;
  }  while(pTemp);
  
}

IsoDirEntry*
isoQueryIsoEntryFromPath(VolData * pVolData,
    char * pszPath)
{
  char * pszPos;
  struct iso_directory_record * idr;
  IsoDirEntry* pIsoDirEntry=NULL;
  IsoDirEntry* pEntry;
  /*  int iExtent=0;*/
  Bool bFirstFound=FALSE;
  IsoDirEntry tempEntry;

  
  logMsg(L_DBG, "Entered isoQueryIsoEntryFromPath(), path is: %s",pszPath);
  memset(&tempEntry,0,sizeof(IsoDirEntry)); 
  idr=(struct iso_directory_record *)pVolData->ipd.root_directory_record;    

  while (1) {
    
    /* Skip the leading separators. */
    while (*pszPath && IS_PATH_SEPARATOR(*pszPath))
      pszPath++;

    
    if (!*pszPath){
      if(bFirstFound)
        break; /* no more components. */
      else {
        pEntry=malloc(sizeof(IsoDirEntry));
        memset(pEntry,0,sizeof(IsoDirEntry)); 
        pEntry->iExtent=pVolData->iRootExtent;
        pEntry->iSize=pVolData->iRootSize;
        pEntry->fstat_buf.st_mode |= S_IFDIR;
        pEntry->flFlags=0;
        pEntry->flFlags|=FILE_READONLY;        
        pEntry->flFlags|=FILE_DIRECTORY;
        memcpy(pEntry->date_buf, idr->date, 9);        
        return pEntry;
      }
    }


    /* Advance to the next separator, or the end. */
    pszPos = pszPath;
    while (*pszPos && !IS_PATH_SEPARATOR(*pszPos))
      pszPos++;

    if(!bFirstFound) {
      /* Read the contents of the root directory. */   

      pIsoDirEntry=getDirEntries(pszPath, pVolData->iRootExtent,
                                 isonum_733((unsigned char *)idr->size), pVolData, 0);
    }
    else {
      pIsoDirEntry=getDirEntries(pszPath, tempEntry.iExtent,
                                 tempEntry.iSize, pVolData, 0);
    }

    if(!pIsoDirEntry) 
      return 0; /* Not found */
   
    /* Look for the current component. */
    for (pEntry = pIsoDirEntry;
         pEntry;
         pEntry = pEntry->pNext)
      /* !!! compareFileNames */
      if ((strlen((char *) pEntry->chrName) == pszPos - pszPath) &&
          (strnicmp((char *) pEntry->chrName, pszPath,
                    pszPos - pszPath) == 0)) 
        break;

    if (!pEntry) {
      /* Path component not found! */
     isoFreeDirEntries(pIsoDirEntry);
      return 0;
    }

    logMsg(L_DBG, "Found entry: %s, extent: %d",pEntry->chrName, pEntry->iExtent);

    memcpy(&tempEntry,pEntry,sizeof(IsoDirEntry));

    isoFreeDirEntries(pIsoDirEntry);
    pszPath = pszPos;
    bFirstFound=TRUE;

  }/* while */

  pEntry=malloc(sizeof(IsoDirEntry));
  memcpy(pEntry, &tempEntry, sizeof(IsoDirEntry));
  return pEntry;
}

int
isoQueryDirExtentFromPath(VolData * pVolData,
   int iRootExtent, char * pszPath, int* pSize)
{
  char * pszPos;
  struct iso_directory_record * idr;
  IsoDirEntry* pIsoDirEntry=NULL;
  IsoDirEntry* pEntry;
  int iExtent;
  int iSize=0;

  logMsg(L_DBG, "Entered isoQueryExtentFromPath(), path is: %s",pszPath);
  iExtent=0;
  if(pSize)
    iSize=pVolData->iRootSize;

  while (1) {
    
    /* Skip the leading separators. */
    while (*pszPath && IS_PATH_SEPARATOR(*pszPath))
      pszPath++;

    if (!*pszPath){
      if(!iExtent)
        iExtent=iRootExtent;
      break; /* no more components. */
    }

    /* Advance to the next separator, or the end. */
    pszPos = pszPath;
    while (*pszPos && !IS_PATH_SEPARATOR(*pszPos))
      pszPos++;

    if(!iExtent) {
      /* Read the contents of the root directory. */
   
      idr=(struct iso_directory_record *)pVolData->ipd.root_directory_record;
      pIsoDirEntry=getDirEntries(pszPath, iRootExtent,
                                 isonum_733((unsigned char *)idr->size), pVolData, 0);
    }
    else {
      pIsoDirEntry=getDirEntries(pszPath, iExtent,
                                 iSize, pVolData, 0);
    }
    
    if(!pIsoDirEntry) 
      return 0; /* Not found */

   
    /* Look for the current component. */
    for (pEntry = pIsoDirEntry;
         pEntry;
         pEntry = pEntry->pNext)
      /* !!! compareFileNames */
      if ((strlen((char *) pEntry->chrName) == pszPos - pszPath) &&
          (strnicmp((char *) pEntry->chrName, pszPath,
                    pszPos - pszPath) == 0)) 
        break;
    
    if (!pEntry) {
      isoFreeDirEntries(pIsoDirEntry);
      return 0;
    }

    logMsg(L_DBG, "Found entry: %s, extent: %d",pEntry->chrName, pEntry->iExtent);

    /* Check if it's a dir */
    if(!S_ISDIR(pEntry->fstat_buf.st_mode)) {
      isoFreeDirEntries(pIsoDirEntry);
      return 0;
    }

    iExtent=pEntry->iExtent;
    iSize=pEntry->iSize;

    isoFreeDirEntries(pIsoDirEntry);
    pszPath = pszPos;
  }/* while */

  if(pSize)
    *pSize=iSize;

  return iExtent;

}



