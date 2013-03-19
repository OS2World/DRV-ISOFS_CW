BASE = .
BASE2 = ..
include $(BASE)/Makefile.incl

SUBDIRS = misc system/$(SYSTEM) corefs libunls \
	ifsdaemon ifsutils

all clean-stuff install:
	for subdir in $(SUBDIRS); do \
	  ( cd $$subdir && $(MAKE) -w $@ ); \
	done

clean:
	rm -f readme.txt
	rm -f readme.html
	rm -f readme.inf
	for subdir in $(SUBDIRS); do \
	  ( cd $$subdir && $(MAKE) -w $@ ); \
	done


all: docs

docs: readme.txt readme.inf readme.html

readme.txt: readme.src
	emxdoc -T -o $@ $<

readme.html: readme.src
	emxdoc -H -o $@ $<

readme.inf: readme.src
	emxdoc -I -o readme.ipf $<
	ipfc readme.ipf $@
	rm readme.ipf

CHECKSUMS:
	md5sum -b `find . -type f` | pgp -staf +clearsig > $@

dist:	Makefile
	-rm -rf ../dist
#	-rm -f ./$(WARPINNAME)
	-mkdir ../dist
	-mkdir $(INSTALLDIR)
	-mkdir ../dist/Docs
	make install
	-cp readme.inf ../dist/Docs/
	-cp COPYING ../dist/Docs/
	-cp ./precompiled/stubfsd.ifs ../dist/bin/
	-cp ./precompiled/install.cmd ../dist/
	-cp ./precompiled/isofs.ins ../dist/
	-cp ./precompiled/uninstal.cmd ../dist/bin/
	-cp FILE_ID.DIZ ../dist/
	-cp readme.1st ../dist/
	-mkdir ../dist/Source
	make clean 
	-cp -R * ../dist/Source/
	cd ../dist && zip -9r $(DISTNAME).zip *
#	wic $(WARPINNAME) -a 1 -r  bin\\*
#	wic $(WARPINNAME) -a 1 -r  Docs\\*
#	wic $(WARPINNAME) -a 2   *.incl
#	wic $(WARPINNAME) -a 2   Makefile
#	wic $(WARPINNAME) -a 2   *.src
#	wic $(WARPINNAME) -a 2   COPYING
#	wic $(WARPINNAME) -a 2   FILE_ID.DIZ
#	wic $(WARPINNAME) -a 2 -r include\\*
#	wic $(WARPINNAME) -a 2 -r warpin\\*
#	wic $(WARPINNAME) -a 2 -r misc\\*
#	wic $(WARPINNAME) -a 2    system\\*
#	wic $(WARPINNAME) -a 2    system\\posix\\*
#	wic $(WARPINNAME) -a 2    system\\os2\\*
#	wic $(WARPINNAME) -a 2 -r corefs\\*
#	wic $(WARPINNAME) -a 2 -r ifsdaemon\\*
#	wic $(WARPINNAME) -a 2 -r ifsdriver\\*
#	wic $(WARPINNAME) -a 2 -r ifsutils\\*
#	wic $(WARPINNAME) -a 2 -r libunls\\*
#	wic $(WARPINNAME) -s ./warpin/isofs.wis
#	zip -9r $(DISTNAME).zip $(WARPINNAME)
#	zip -9r $(DISTNAME).zip readme.txt
#	zip -9r $(DISTNAME).zip FILE_ID.DIZ


