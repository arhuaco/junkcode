bin_PROGRAMS=  bin2dec diehardc checkone checkmany float2txt nbest rng1 ga

bin2dec_SOURCES    = bin2dec.c
rng1_SOURCES       = rng1.c util.h caiterate.c caiterate.h \
                     entropy_meter.c entropy_meter.h
rng1_LDADD     = -lm

diehardc_SOURCES   = diehardc.c
diehardc_LDADD     = -lm

checkone_SOURCES    = check-one.c \
                      caiterate.c caiterate.h \
											util.h \
											entropy_meter.c entropy_meter.h
checkone_LDADD = -lm

checkmany_SOURCES   = check-many.c \
                      caiterate.c caiterate.h \
											util.h \
											entropy_meter.c entropy_meter.h
checkmany_LDADD = -lm

float2txt_SOURCES   = float2txt.c

nbest_SOURCES  = nbest.c util.h
nbest_LDADD    = -lm

#Building with galib is a pain in the neck.
ga_SOURCES         = ga.c fitness.c fitness.h entropy_meter.c entropy_meter.h caiterate.c caiterate.h  util.h
ga_LDADD           = -lmiga -lm

#Building with galib is a pain in the neck.
#ga_SOURCES         = ex20.C fitness.c fitness.h entropy_meter.c entropy_meter.h caaiterate.c caiterate.h  util.h
#ga_LDADD           = -lga -lm
