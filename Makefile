all: applet overlay

applet:
	$(MAKE) -f Makefile.applet

overlay:
	$(MAKE) -f Makefile.overlay

clean:
	$(MAKE) -f Makefile.applet clean
	$(MAKE) -f Makefile.overlay clean

.PHONY: all applet overlay
