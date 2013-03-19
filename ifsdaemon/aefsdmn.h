/* aefsdmn.h -- Header file for the daemon code.
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

#ifndef _AEFSDMN_H
#define _AEFSDMN_H

#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "iso9660.h"

#include "stubfsd.h"

#include "aefsdint.h"

#include "corefs.h"

#define sector_offset 0
#define use_joliet 1

typedef struct _ServerData ServerData;

struct _ServerData{
      Bool fQuit;
      
      /* Mutex semaphore guarding against concurrent access. */
      HMTX hmtxGlobal;
      
      /* Exchange buffers. */
      PFSREQUEST pRequest;
      PFSDATA pData;

      /* Head of linked list of volumes. */
      VolData * pFirstVolume;
#if 0
      /* Default values for newly attached volumes.  These can be
         changed at startup or through aefsparm.  They cannot
         be changed for attached volumes. */
      int cMaxCryptedFiles;
      int cMaxOpenStorageFiles; /* not too high! */
      int csMaxCached;
#endif
};

typedef struct _IsoDirEntry IsoDirEntry;
struct _IsoDirEntry {
  IsoDirEntry * pNext;
  int cbName;
  int flFlags;
  int iExtent; /* Extent of this entry */
  int iSize;   /* Len of extent */
  int iParentExtent; /* Extent of the parent */
  char chrFullPath[256]; /* Is this the right len for a path???? */
  char chrName[256];
  struct stat fstat_buf;
  char date_buf[9];
};


struct _SearchData {
      CHAR szName[CCHMAXPATH];
      ULONG flAttr;
      IsoDirEntry * pFirstInIsoDir;
      IsoDirEntry isoDot , isoDotDot;
  IsoDirEntry * pIsoNext;
      int iNext;
};

struct _OpenFileData {
      CryptedFileID idFile;
  IsoDirEntry* pIsoEntry;
      /* Path name through which the file was opened. */
      CHAR szName[CCHMAXPATH];
      CryptedFileID idDir;
  int iExtent;
  int iSize;

};

struct _VolData {
      ServerData * pServerData;
      char chDrive;
      
      CryptedVolume * pVolume; /* copy of pSuperBlock->pVolume */
      CryptedFileID idRoot; /* copy of pSuperBlock->idRoot */

      Bool fReadOnly;

  FILE * isoFile; /* The mounted ISO file */
  char   fileName[CCHMAXPATH];
  struct iso_primary_descriptor ipd; /* and it's descriptor */
  char  chrCDName[34];
  int iRootExtent; /* The root extent of the ISO */
  int iRootSize;
  IsoDirEntry* curDir; /* The linked list of the contents of the current dir */
  int ucs_level;
  int iSectorOffset;
  char szCharSet[CCHMAXPATH];
  struct nls_table *nls;
      /* Statistics. */
      int cOpenFiles;
      int cSearches;

      /* Next and previous elements in linked list of volumes. */
      VolData * pNext;
      VolData * pPrev;
};


/* Message severity codes. */
#define L_FATAL  1
#define L_EVIL   2
#define L_ERR    3
#define L_WARN   4
#define L_DBG    9


/* Additional value for DOS attribute fields. */
#define FILE_NON83 0x40


/* Flush values for stampFileAndFlush(). */
#define SFAF_NOFLUSH    0 /* don't flush anything */
#define SFAF_FLUSHINFO  1 /* flush the info sector */
#define SFAF_FLUSHALL   2 /* flush all the file's sectors */


/* Global functions. */

IsoDirEntry* isoQueryIsoEntryFromPath(VolData * pVolData, char * pszPath);

int isoQueryDirExtentFromPath(VolData * pVolData,
                              int iRootExtent, char * pszPath, int* pSize);

IsoDirEntry* getDirEntries(char	*rootname, int	extent,	int	len, VolData  * pVolData, int iOnlyDirs);

int isoEntryTimeToOS2(IsoDirEntry* pIsoDirEntry, FDATE * pfdate, FTIME * pftime);

int isonum_723 (char	*p);

int isonum_733 (unsigned char	*p);

void logMsg(int level, char * pszMsg, ...);

int processArgs(ServerData * pServerData, int argc, char * * argv,
   int startup);

int verifyString(char * pszStr, int cbMaxLen);

#define VERIFYFIXED(str) verifyString(str, sizeof(str))

int verifyPathName(char * pszName);

Bool hasNon83Name(char * pszName);

APIRET coreResultToOS2(CoreResult cr);

void splitPath(char * pszFull, char * pszPrefix, char * pszLast);

void logsffsi(struct sffsi * psffsi);

USHORT makeDOSAttr(Bool fHidden, CryptedFileInfo * pInfo);

APIRET storeFileInfo(
   CryptedVolume * pVolume, /* DosFindXXX level 3 only */
   CryptedFileID idFile, /* DosFindXXX level 3 only */
   PGEALIST pgeas, /* DosFindXXX level 3 only */
   char * pszFileName, /* DosFindXXX only */
   Bool fHidden,
   char * * ppData,
   ULONG * pcbData,
   ULONG ulLevel,
   ULONG flFlags,
   int iNext,
   IsoDirEntry * pEntry);

APIRET deleteFile(VolData * pVolData, char * pszFullName);

int compareEANames(char * pszName1, char * pszName2);

APIRET storeEAsInFEAList(CryptedVolume * pVolume,
                         CryptedFileID idFile, 
                         PGEALIST pgeas, ULONG cbData, char * pData);
  

CoreTime curTime();

int coreTimeToOS2(CoreTime time, FDATE * pfdate, FTIME * pftime);

int os2TimeToCore(FDATE fdate, FTIME ftime, CoreTime * ptime);

APIRET commitVolume(VolData * pVolData);


/* FSD functions. */
APIRET fsFsCtl(ServerData * pServerData, struct fsctl * pfsctl);
APIRET fsAttach(ServerData * pServerData, struct attach * pattach);
APIRET fsIOCtl(ServerData * pServerData, struct ioctl * pioctl);
APIRET fsFsInfo(ServerData * pServerData, struct fsinfo * pfsinfo);
APIRET fsFlushBuf(ServerData * pServerData,
   struct flushbuf * pflushbuf);
APIRET fsShutdown(ServerData * pServerData,
   struct shutdown * pshutdown);
APIRET fsOpenCreate(ServerData * pServerData,
   struct opencreate * popencreate);
APIRET fsClose(ServerData * pServerData, struct close * pclose);
APIRET fsRead(ServerData * pServerData, struct read * pread);
APIRET fsWrite(ServerData * pServerData, struct write * pwrite);
APIRET fsChgFilePtr(ServerData * pServerData,
   struct chgfileptr * pchgfileptr);
APIRET fsNewSize(ServerData * pServerData, struct newsize * pnewsize);
APIRET fsFileAttribute(ServerData * pServerData,
   struct fileattribute * pfileattribute);
APIRET fsFileInfo(ServerData * pServerData,
   struct fileinfo * pfileinfo);
APIRET fsCommit(ServerData * pServerData, struct commit * pcommit);
APIRET fsPathInfo(ServerData * pServerData,
   struct pathinfo * ppathinfo);
APIRET fsDelete(ServerData * pServerData, struct delete * pdelete);
APIRET fsMove(ServerData * pServerData, struct move * pmove);
APIRET fsChDir(ServerData * pServerData, struct chdir * pchdir);
APIRET fsMkDir(ServerData * pServerData, struct mkdir * pmkdir);
APIRET fsRmDir(ServerData * pServerData, struct rmdir * prmdir);
APIRET fsFindFirst(ServerData * pServerData,
   struct findfirst * pfindfirst);
APIRET fsFindNext(ServerData * pServerData,
   struct findnext * pfindnext);
APIRET fsFindFromName(ServerData * pServerData,
   struct findfromname * pfindfromname);
APIRET fsFindClose(ServerData * pServerData,
   struct findclose * pfindclose);
APIRET fsProcessName(ServerData * pServerData,
   struct processname * pprocessname);

#endif /* !_AEFSDMN_H */




