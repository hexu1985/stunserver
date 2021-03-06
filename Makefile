.PHONY: all everything copybin debug clean

all: everything copybin

everything:
	cp common.inc.linux common.inc
	$(MAKE) $(T) --directory=common
	$(MAKE) $(T) --directory=stuncore
	$(MAKE) $(T) --directory=networkutils
	$(MAKE) $(T) --directory=export
	$(MAKE) $(T) --directory=testcode
	$(MAKE) $(T) --directory=client
	$(MAKE) $(T) --directory=server
	$(MAKE) $(T) --directory=testexport

copybin: everything
	rm -f ./stunserver ./stunclient ./stuntestcode
	rm -f ./lib/*.a
	cp server/stunserver .
	cp client/stunclient .
	cp testcode/stuntestcode .
	cp export/libexport.a ./lib/
	cp stuncore/libstuncore.a ./lib/
	cp networkutils/libnetworkutils.a ./lib/
	cp common/libcommon.a ./lib/

debug: T := debug
debug: all

profile: T := profile
profile: all

clean:	T := clean
clean: everything
	rm -f stunserver stunclient stuntestcode



