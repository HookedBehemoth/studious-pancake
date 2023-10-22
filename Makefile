export GITHASH 		:= $(shell git rev-parse --short HEAD)
export VERSION 		:= 1.0.0

all: applet overlay

applet:
	$(MAKE) -f Makefile.applet

overlay:
	$(MAKE) -f Makefile.overlay

clean:
	$(MAKE) -f Makefile.applet clean
	$(MAKE) -f Makefile.overlay clean

dist: all
	mkdir -p dist/switch/.overlays
	cp overlay/studious-pancake.ovl dist/switch/.overlays/
	cp applet/studious-pancake.nro dist/switch/
	cd dist; zip -r studious-pancake-$(VERSION)-$(GITHASH).zip ./**/; cd ../;

.PHONY: all applet overlay
