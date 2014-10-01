GCC_BIN = 
PROJECT = test
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
INCLUDE_PATHS = -I.  
LIBRARY_PATHS = -L.
LIBRARIES = 
LINKER_SCRIPT = 
 
DEPFILES = $(patsubst %.o,%.d,$(OBJECTS))
 
CC = $(GCC_BIN)gcc
CC_FLAGS = -g -Wall
 
CC_FLAGS += -MMD -MP
 
ONLY_C_FLAGS = 
ONLY_CPP_FLAGS = -
CC_SYMBOLS = 
  
all: $(PROJECT)
 
clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f $(DEPFILES)
 
.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) $(ONLY_C_FLAGS) $(INCLUDE_PATHS) -o $@ $<

 
$(PROJECT): $(OBJECTS)
	cp $(PROJECT).o $(PROJECT)

 -include $(DEPFILES)