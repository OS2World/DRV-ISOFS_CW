/* fileio.c -- File I/O.
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

#include <assert.h>

#include "aefsdmn.h"

APIRET fsRead(ServerData * pServerData, struct read * pread)
{
   ULONG cbLen = pread->cbLen;
   size_t bRead;
   size_t offset;
   size_t bytesRead=0;
   size_t toRead;
   int sectorOffset=pread->pVolData->iSectorOffset;
   int iStartExtent;
   unsigned char buff[2048];
   char *pData=(char*) pServerData->pData;
   int iSeek;

   pread->cbLen = 0; /* on output, # of bytes read */ 
   sectorOffset=0;

   if (cbLen > pread->sffsi.sfi_size)
     cbLen = pread->sffsi.sfi_size;

   if(cbLen > (pread->sffsi.sfi_size-pread->sffsi.sfi_position))
     cbLen=(pread->sffsi.sfi_size-pread->sffsi.sfi_position);

   logMsg(L_DBG,
      "FS_READ, cbLen=%hu, fsIOFlag=%04hx",
      cbLen, pread->fsIOFlag);
   logsffsi(&pread->sffsi);

   iStartExtent=pread->pOpenFileData->iExtent+pread->sffsi.sfi_position/2048;
    
   iSeek=fseek(pread->pVolData->isoFile,(iStartExtent - sectorOffset) << 11, SEEK_SET);
   bRead=fread( &buff, sizeof(char), 2048,pread->pVolData->isoFile);/* Read first Extent */
   
   logMsg(L_DBG, "bRead=%ld", bRead);
 
#if 0
   assert(bRead = 2048);
#endif

   offset=pread->sffsi.sfi_position%2048;
   toRead=( (2048-offset) > cbLen ? cbLen : (2048-offset));
   logMsg(L_DBG, "toRead=%ld, offset=%d, cbLen:%d", toRead,offset,cbLen);
   memcpy(pData, &buff[offset], toRead);

   bytesRead+=toRead;
   cbLen-=toRead;
   iStartExtent++;
   logMsg(L_DBG, "cbLen=%ld, iStartExtent=%d", cbLen,iStartExtent);

   while(cbLen > 0)
    {
      pData+=toRead;
      fseek(pread->pVolData->isoFile, (long)(iStartExtent - sectorOffset) << 11, 0);
      toRead = (cbLen > sizeof(buff) ? sizeof(buff) : cbLen);
      bRead=fread(buff, sizeof(char), toRead, pread->pVolData->isoFile);
      logMsg(L_DBG, "bRead=%ld", bRead);
      cbLen -= toRead;
      iStartExtent++;
      memcpy(pData, &buff, toRead);
      bytesRead+=toRead;
    }


   pread->cbLen = (USHORT) bytesRead;
   pread->sffsi.sfi_position += bytesRead;
   pread->sffsi.sfi_tstamp |= (ST_SREAD | ST_PREAD);

   return NO_ERROR;
}


APIRET fsWrite(ServerData * pServerData, struct write * pwrite)
{
   ULONG cbLen = pwrite->cbLen;

   pwrite->cbLen = 0; /* on output, # of bytes written */ 

   logMsg(L_DBG,
      "FS_WRITE, cbLen=%hu, fsIOFlag=%04hx",
      cbLen, pwrite->fsIOFlag);

   return ERROR_WRITE_PROTECT;

}

APIRET fsChgFilePtr(ServerData * pServerData,
   struct chgfileptr * pchgfileptr)
{
   LONG lNewPos;
   
   logMsg(L_DBG,
      "FS_CHGFILEPTR, ibOffset=%ld, usType=%hu, fsIOFlag=%04hx",
      pchgfileptr->ibOffset, pchgfileptr->usType, pchgfileptr->fsIOFlag);
   logsffsi(&pchgfileptr->sffsi);

   switch (pchgfileptr->usType) {

      case FILE_BEGIN:
         pchgfileptr->sffsi.sfi_position = pchgfileptr->ibOffset;
         return NO_ERROR;

      case FILE_CURRENT:
         lNewPos = (LONG) pchgfileptr->sffsi.sfi_position +
            pchgfileptr->ibOffset;
         if (lNewPos < 0)
            return ERROR_NEGATIVE_SEEK;
         pchgfileptr->sffsi.sfi_position = lNewPos;
         return NO_ERROR;

      case FILE_END:
         lNewPos = (LONG) pchgfileptr->sffsi.sfi_size +
            pchgfileptr->ibOffset;
         if (lNewPos < 0)
            return ERROR_NEGATIVE_SEEK;
         pchgfileptr->sffsi.sfi_position = lNewPos;
         return NO_ERROR;

      default:
         logMsg(L_EVIL, "unknown FS_CHGFILEPTR type: %d",
            pchgfileptr->usType);
         return ERROR_NOT_SUPPORTED;
   }
}


APIRET fsNewSize(ServerData * pServerData, struct newsize * pnewsize)
{
   
   logMsg(L_DBG,
      "FS_NEWSIZE, cbLen=%ld, fsIOFlag=%04hx",
      pnewsize->cbLen, pnewsize->fsIOFlag);

   return ERROR_WRITE_PROTECT;

}


APIRET fsCommit(ServerData * pServerData, struct commit * pcommit)
{
   logMsg(L_DBG, "FS_COMMIT, usType=%hd, fsIOFlag=%hx",
      pcommit->usType, pcommit->fsIOFlag);
   logsffsi(&pcommit->sffsi);

   return NO_ERROR;   
}


APIRET fsIOCtl(ServerData * pServerData, struct ioctl * pioctl)
{
   logMsg(L_DBG,
      "FS_IOCTL, usCat=%hd, usFunc=%hd, cbParm=%d, cbMaxData=%d",
      pioctl->usCat, pioctl->usFunc, pioctl->cbParm,
      pioctl->cbMaxData);
   logsffsi(&pioctl->sffsi);

   return ERROR_NOT_SUPPORTED;
}





