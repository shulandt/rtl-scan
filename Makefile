
PREFIX ?= /usr
DESTDIR ?=
BINDIR ?= $(PREFIX)/bin

all: rtl-scan

rtl-scan: *.cpp 
	g++ *.cpp -o rtl-scan
clean:
	rm -f rtl-scan

install: rtl-scan
	@install -v -d "$(DESTDIR)$(BINDIR)" && install -s -m 0755 -v rtl-scan "$(DESTDIR)$(BINDIR)/rtl-scan"
