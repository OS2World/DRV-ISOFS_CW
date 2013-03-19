/*           Install Isofs V0.2.0 
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

/* Get curent directory */
curdir = directory()

IF SysIni('USER','cwinst','version')\='ERROR:' THEN DO
	rc=SysSetObjectData(curdir'\isofs.ins',"OPEN=DEFAULT");
	if rc==1 THEN
		Exit
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

call installFS
call createObjects
SAY 'The old Config.sys was saved as config.iso.'
SAY 'You have to reboot so the filesystem can be loaded.'
EXIT


banner:
SAY ''
SAY 'Isofs V0.2.1'
SAY
SAY '(C) Chris Wohlgemuth 2000-2001'
SAY
SAY 'http://www.geocities.com/SiliconValley/Sector/5785/'
SAY ''
SAY '----------------------'
SAY ''
SAY 'This installable filesystem gives you read-only access to'
SAY 'ISO image files used for burning on CD.'
SAY 'See readme.inf for more information.'
SAY ''
SAY 'This install skript will add the following lines to your'
SAY 'config.sys:'
SAY ''
SAY 'IFS=<install-path>\stubfsd.ifs ISOFS'
SAY 'RUN=<install-path>\isofsdmn.exe'
SAY ''
return

installFS:

 bootdrv = translate(value('PATH', , 'os2environment'))
 parse var bootdrv bootdrv':\OS2;'rest
 bootdrv = substr(bootdrv, length(bootdrv))

 configsys=bootdrv||':\config.sys'

 SAY 'Modifying 'configsys'...'
   i=0
   do while lines(configsys) <> 0
	i=i+1
	config.i=linein(configsys)
   END
   config.0=i
   call stream configsys, 'c', 'close'
   
   '@copy 'configsys' 'bootdrv':\config.iso 2>NUL 1>NUL'
   if rc\=0 THEN DO
	SAY 'Cannot backup the current config.sys. Aborting...'
	EXIT
   END

   rc=lineout(configsys, 'REM *** Isofs V0.2.1 ***')
   rc=lineout(configsys, 'IFS='curdir'\bin\stubfsd.ifs ISOFS')
   rc=lineout(configsys, 'RUN='curdir'\bin\isofsdmn.exe')
Return

createObjects:
setupstring='OBJECTID=<ISOFS_FOLDER>'
if SysCreateObject("WPFolder","Isofs V0.2.1","<WP_DESKTOP>",setupstring,"update")<>1 then
             do
                say ''
                say 'Can''t create the isofs folder!'
		return
             end


setupstring='SHADOWID=<ISOFS_FOLDER>;OBJECTID=<ISOFS_FLDRSHADOW>'
if SysCreateObject("WPShadow","Isofs V0.2.1","<CD_WRITING>",setupstring,"update")<>1 then
             do
             end

setupstring='EXENAME=view.exe;STARTUPDIR='curDir'\docs;PARAMETERS='curDir'\docs\readme.inf;OBJECTID=<ISOFS_README>'
say 'Creating an object for the readme...'
if SysCreateObject("WPProgram","Isofs Readme","<ISOFS_FOLDER>",setupstring,"update")<>1 then
	do
              say ''
              say 'Cannot create an object for the readme!'
              say '' 
        end

setupstring='EXENAME='curdir'\bin\mntisofs.exe;STARTUPDIR='curDir'\bin;PARAMETERS=[drive letter:] %*;MINIMIZED=YES;MINVIEW=HIDE;OBJECTID=<ISOFS_MOUNT>'
say 'Creating an object for mounting...'
if SysCreateObject("WPProgram","Mount iso file^(Drop the file here)","<ISOFS_FOLDER>",setupstring,"update")<>1 then
	do
              say ''
              say 'Cannot create an object for mounting an iso file!'
              say '' 
        end

setupstring='EXENAME='curdir'\bin\umntiso.exe;STARTUPDIR='curDir'\bin;PARAMETERS=[drive letter:];MINIMIZED=YES;MINVIEW=HIDE;OBJECTID=<ISOFS_UNMOUNT>'
say 'Creating an object for unmounting...'
if SysCreateObject("WPProgram","Unmount iso file","<ISOFS_FOLDER>",setupstring,"update")<>1 then
	do
              say ''
              say 'Cannot create an object for unmounting an iso file!'
              say '' 
        end

setupstring='EXENAME='curdir'\bin\uninstal.cmd;STARTUPDIR='curDir'\bin;PARAMETERS=2;OBJECTID=<ISOFS_UNINSTALL>'
say 'Creating an object for uninstalling...'
if SysCreateObject("WPProgram","Uninstall Iso-FS^V0.2.1","<ISOFS_FOLDER>",setupstring,"update")<>1 then
	do
              say ''
              say 'Cannot create an object for uninstalling!'
              say '' 
        end


rc=SysSetObjectdata('<ISOFS_FOLDER>','OPEN=DEFAULT')

return
