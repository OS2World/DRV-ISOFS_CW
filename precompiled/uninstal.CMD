/*           Uninstall Isofs V0.2.0 
 *
 *           (C) Chris Wohlgemuth 2000-2001
 *
 *   http://www.geocities.com/SiliconValley/Sector/5785/
*/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

parse source . . progname .

/* Get curent directory */
curdir = directory()

IF WORDS(ARG(1))=1 THEN DO
  '@start /C /F 'progname' 'ARG(1)' 2' 
  EXIT
END

if WORD(ARG(1), 1)='1' THEN DO
installer=SysIni('USER','cwinst','instdir')
IF installer\='ERROR:' THEN DO
	rc=SysSetObjectData(installer'\install.exe',"OPEN=DEFAULT");
	if rc==1 THEN
		Exit
END
END

'cls'

call banner

say 'Press <ENTER> to continue or Q for exit.'
say ''
say '>'
parse value SysCurPos() with aktzeile aktspalte
newposition=SysCurPos(aktzeile-1,2)
pull taste 
IF taste='Q' THEN EXIT

call uninstallFS
call deleteObjects
SAY ''
SAY 'The old Config.sys was saved as config.iso.'
SAY 'You have to reboot so the filesystem will be unloaded.'
SAY 'After reboot delete the isofs directory.'
'@PAUSE'
EXIT


banner:
SAY ''
SAY 'Isofs V0.2.0'
SAY
SAY '(C) Chris Wohlgemuth 2000-2001'
SAY
SAY 'http://www.geocities.com/SiliconValley/Sector/5785/'
SAY ''
SAY '----------------------'
SAY ''
SAY 'This uninstall skript will remove the following lines from your'
SAY 'config.sys:'
SAY ''
SAY 'IFS=<install-path>\stubfsd.ifs ISOFS'
SAY 'RUN=<install-path>\isofsdmn.exe'
SAY ''
return

uninstallFS:

 bootdrv = translate(value('PATH', , 'os2environment'))
 parse var bootdrv bootdrv':\OS2;'rest
 bootdrv = substr(bootdrv, length(bootdrv))

 configsys=bootdrv||':\config.sys'

 SAY 'Modifying 'configsys'...'
   i=0
   do while lines(configsys) <> 0
	i=i+1
	config.i=linein(configsys)
         IF substr(TRANSLATE(config.i), 1, 4) = "IFS=" then do
            if pos("STUBFSD.IFS", TRANSLATE(config.i)) <> 0 then do
		config.i=''
            end
         end
         IF substr(TRANSLATE(config.i), 1, 4) = "RUN=" then do
            if pos("ISOFSDMN.EXE", TRANSLATE(config.i)) <> 0 then do
		config.i=''
            end
         end
   END
   config.0=i
   call stream configsys, 'c', 'close'

   '@copy 'configsys' 'bootdrv':\config.iso 2>NUL 1>NUL'
   if rc\=0 THEN DO
	SAY 'Cannot backup the current config.sys. Aborting...'
	EXIT
   END
   '@del 'configsys' 2>NUL 1>NUL'
   if rc\=0 THEN DO
	SAY 'Cannot change the current config.sys. Aborting...'
	EXIT
   END
	
   DO i=1 to config.0
     rc=lineout(configsys, config.i)
   END
Return

deleteObjects:
if SysDestroyObject("<ISOFS_README>")<>1 then
	do
              say ''
              say 'Cannot delete the object for the readme!'
        end

if SysDestroyObject("<ISOFS_MOUNT>")<>1 then
	do
              say ''
              say 'Cannot delete the object for mounting an iso file!'
        end

if SysDestroyObject("<ISOFS_UNMOUNT>")<>1 then
	do
              say ''
              say 'Cannot delete the object for unmounting an iso file!'
        end

if SysDestroyObject("<ISOFS_UNINSTALL>")<>1 then
	do
              say ''
              say 'Cannot delete the object for uninstalling!'
        end


if SysDestroyObject("<ISOFS_FLDRSHADOW>")<>1 then
             do

		END

if SysDestroyObject("<ISOFS_FOLDER>")<>1 then
             do
                say ''
                say 'Can''t delete the isofs folder!'
		return
             end
return
