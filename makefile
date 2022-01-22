LIB=common subject behaviorSubject subscription
LIBSRC=$(addprefix src/,$(addsuffix .cpp, $(LIB)))
LIBOBJ=$(LIBSRC:.cpp=.o)

ifeq ($(OS),Windows_NT) #Merci windows d'avoir remplacé le "/" par "\" !
    detected_OS = Windows
	/="\"
	RM_CMD=del /Q
	CAT_CMD=type
else
    detected_OS = $(shell uname)
	/=/
	RM_CMD=rm
	CAT_CMD=cat
endif

prog: bin/prog

bin/prog: test.cpp
	g++ test.cpp -o bin/prog -L./lib


lib: lib/libobservables.a

lib/libobservables.a: $(LIBOBJ)
	ar rcs lib/libobservables.a $(LIBOBJ)

%.o: %.cpp
	gcc -c $< -o $@


clean:
	$(RM_CMD) src$/*.o

remove:
	$(RM_CMD) bin$/*
	$(RM_CMD) lib$/*

.PHONY: prog lib clean remove