BINARY=thief
CODEDIRS= ./ ./net ./lib 
# include code layers 
INCDIRS= $(CODEDIRS)

CC=gcc
OPT=-O0
DEPFLAGS=-MP -MD
CFLAGS=-Wall -Wextra -g $(foreach D, $(INCDIRS), -I$(D)) $(OPT) $(DEPFLAGS)

CFILES=$(foreach D, $(CODEDIRS), $(wildcard $(D)/*.c)) 
OFILES=$(patsubst %.c, %.o, $(CFILES))
DFILES=$(patsubst %.c, %.d, $(CFILES))

ASMFLAGS=-Wall -Wextra -g $(foreach D, $(INCDIRS), -I$(D)) $(OPT) 

SFILES=$(wildcard ./*.s)

all: $(BINARY)

$(BINARY): $(OFILES)
	$(CC) -o $@ $^ 

%.o: $.c
	$(CC) $(CFLAGS) -c -o $@ $^
	
clean:
	rm -rf $(OFILES) $(DFILES) $(SFILES)

asmdump: $(CFILES) 
	$(CC) $(ASMFLAGS) -S $^

run: 
	@make -s $(BINARY) 
	@./$(BINARY) 

