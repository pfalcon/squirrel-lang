SQUIRREL= ..


OUT= $(SQUIRREL)/bin/sq
INCZ= -I$(SQUIRREL)/include -I. -I$(SQUIRREL)/sqlibs
LIBZ= -L$(SQUIRREL)/lib 
LIB= -lsquirrel -lsqstdlib -lphysfs

OBJS= sq.o
	
SRCS= sq.c
	
	
sq32:
	g++ -O2 -s -fno-exceptions -fno-rtti -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)

sqprof:
	g++ -O2 -pg -fno-exceptions -fno-rtti -g3 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS) #-pie -gstabs 
	
sq64:
	g++ -O2 -s -m64 -fno-exceptions -fno-rtti -D_SQ64 -o $(OUT) $(SRCS) $(INCZ) $(LIBZ) $(LIB) $(DEFS)
