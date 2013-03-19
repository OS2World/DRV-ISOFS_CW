/* corefs.h -- Header file to the system-independent FS code.
   Copyright (C) 1999 Eelco Dolstra (edolstra@students.cs.uu.nl).

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

#ifndef _COREFS_H
#define _COREFS_H

#include "types.h"

typedef struct _CryptedVolume CryptedVolume;

typedef unsigned long CryptedFileID;
typedef unsigned long SectorNumber;
typedef unsigned long CryptedFilePos;


/*
 * Error codes
 */

typedef int CoreResult;

#define CORERC_OK                  0
#define CORERC_FILE_NOT_FOUND      1
#define CORERC_NOT_ENOUGH_MEMORY   2
#define CORERC_FILE_EXISTS         3
#define CORERC_INVALID_PARAMETER   4
#define CORERC_INVALID_NAME        5
#define CORERC_BAD_CHECKSUM        7
#define CORERC_STORAGE             8
#define CORERC_BAD_INFOSECTOR      9
#define CORERC_NOT_DIRECTORY       10
#define CORERC_BAD_DIRECTORY       11
#define CORERC_BAD_TYPE            12
#define CORERC_BAD_EAS             13
#define CORERC_CACHE_OVERFLOW      14
#define CORERC_READ_ONLY           15
#define CORERC_ISF_CORRUPT         16
#define CORERC_ID_EXISTS           17


/*
 * Sector data encryption/decryption
 */

#define SECTOR_SIZE 512
#define RANDOM_SIZE 4
#define CHECKSUM_SIZE 4
#define NONPAYLOAD_SIZE (CHECKSUM_SIZE + RANDOM_SIZE)
#define PAYLOAD_SIZE (SECTOR_SIZE - NONPAYLOAD_SIZE)

/* Flags for encryption/decryption. */
#define CCRYPT_USE_CBC 1


/*
 * Low-level volume stuff
 */

#define MAX_VOLUME_BASE_PATH_NAME 256

typedef struct {
      int flCryptoFlags; /* CCRYPT_* */
      int flOpenFlags; /* SOF_* */
      int cMaxCryptedFiles; /* > 0 */
      int cMaxOpenStorageFiles; /* > 0, <= cMaxCryptedFiles */
      int csMaxCached; /* > 0 */
      int csIOGranularity; /* > 0, <= csMaxCached */
      int csISFGrow; /* > 0 */
      char * pszPathSep; /* presently not used */
      int acbitsDivision[32]; /* presently not used */
      void (* dirtyCallBack)(CryptedVolume * pVolume, Bool fDirty);
      void * pUserData;
} CryptedVolumeParms;

/* Flags for encrypted files (CryptedFileInfo.flFlags).  These are
   equal to the Unix flags.  Most of them are meaningless to the OS/2
   FSD. */

#define CFF_EXTEAS 04000000 /* file has external EAs */

#define CFF_OS2A   02000000 /* file has been modified */
#define CFF_OS2S   01000000 /* system file */

#define CFF_IFMT   00370000
#define CFF_IFEA   00200000
#define CFF_IFSOCK 00140000
#define CFF_IFLNK  00120000
#define CFF_IFREG  00100000
#define CFF_IFBLK  00060000
#define CFF_IFDIR  00040000
#define CFF_IFCHR  00020000
#define CFF_IFIFO  00010000

#define CFF_ISUID  00004000
#define CFF_ISGID  00002000
#define CFF_ISVTX  00001000

#define CFF_ISLNK(m)      (((m) & CFF_IFMT) == CFF_IFLNK)
#define CFF_ISREG(m)      (((m) & CFF_IFMT) == CFF_IFREG)
#define CFF_ISDIR(m)      (((m) & CFF_IFMT) == CFF_IFDIR)
#define CFF_ISCHR(m)      (((m) & CFF_IFMT) == CFF_IFCHR)
#define CFF_ISBLK(m)      (((m) & CFF_IFMT) == CFF_IFBLK)
#define CFF_ISFIFO(m)     (((m) & CFF_IFMT) == CFF_IFIFO)
#define CFF_ISSOCK(m)     (((m) & CFF_IFMT) == CFF_IFSOCK)
#define CFF_ISEA(m)       (((m) & CFF_IFMT) == CFF_IFEA)

#define CFF_IRWXU 00700
#define CFF_IRUSR 00400
#define CFF_IWUSR 00200
#define CFF_IXUSR 00100

#define CFF_IRWXG 00070
#define CFF_IRGRP 00040
#define CFF_IWGRP 00020
#define CFF_IXGRP 00010

#define CFF_IRWXO 00007
#define CFF_IROTH 00004
#define CFF_IWOTH 00002
#define CFF_IXOTH 00001


/* Time type.  Number of seconds since 00:00:00 1-Jan-1970 UTC.
   Should last till 2106 or so.  0 means unknown. */
typedef uint32 CoreTime;


/* Note: fields marked as "ignored" are ignored by
   coreCreateBaseFile() in the structure passed in. */
typedef struct {
      uint32 flFlags;
      
      int cRefs; /* reference count */

      CryptedFilePos cbFileSize;
      SectorNumber csAllocated; /* ignored */
      SectorNumber csSet; /* ignored */

      CoreTime timeCreation;
      CoreTime timeAccess;
      CoreTime timeWrite;

      CryptedFileID idParent; /* directories and EA files only! */
      
      CryptedFilePos cbEAs; /* ignored */
      CryptedFileID idEAFile; /* ignored */
} CryptedFileInfo;


/*
 * Directories
 */

typedef struct _CryptedDirEntry CryptedDirEntry;

struct _CryptedDirEntry {
      CryptedDirEntry * pNext;
      int cbName;
      octet * pabName; /* zero terminated (not incl. in cbName) */
      CryptedFileID idFile;
      int flFlags;
};

/* The on-disk structure of directory entries is: a flag byte, the
   file ID, the length of the file name (4 bytes), and the file name.
   The list of entries is zero-terminated.  A zero-length directory
   file denotes an empty directory. */

/* Flags for CryptedDirEntry.flFlags. */
#define CDF_NOT_EOL           1 /* on-disk only */
#define CDF_HIDDEN            2  


/*
 * Extended attributes
 */

typedef struct _CryptedEA CryptedEA;

struct _CryptedEA {
      CryptedEA * pNext;
      char * pszName;
      int cbValue;
      octet * pabValue;
      int flFlags;
};

#endif /* !_COFEFS_H */




