CHICKENCONFIG=chicken-config
CFLAGS=`$(CHICKENCONFIG) -cflags`

# Older versions of Chicken required -extra-libs to be passed to chicken-config
# in the following line.  Add it if it fails to build.

LDFLAGS=`$(CHICKENCONFIG) -libs`
CHICKEN=chicken
#CHICKENFLAGS=-require-extension syntax-case

all: pcgotchi
pcgotchi: pcgotchi.o

%.c: %.scm
	$(CHICKEN) $< $(CHICKENFLAGS)

.PHONY: clean
clean:
	rm -f *.[oc] *~ pcgotchi
