-include Platform.$(PLATFORM)

.PHONY: all

NETPROJ = 
ifneq "$(realpath /usr/bin/gmcs)" ""
endif

all: $(NETPROJ) Maestro 


.PHONY: Maestro
Maestro:
	$(MAKE) -C ../Maestro
