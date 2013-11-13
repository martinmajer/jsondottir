# Shared library extension
ifeq ($(OS),Windows_NT)
	DLLEXT := .dll
	EXEEXT := .exe
else
	DLLEXT := .so
	EXEEXE := 
endif

CFLAGS := -std=c99 -Wall -O2

DLL := json$(DLLEXT)
TEST := unit_test$(EXEEXT)

all: lib test

lib: json.o json_debug.o json_error.o json_object.o json_reader.o json_tokenizer.o
	gcc -o $(DLL) $^ -shared
	
%.o: %.c
	gcc -std=c99 -Wall -O2 -o $@ -c $< -fPIC
	
test:
	gcc $(CFLAGS) -DJSON_DEBUG *.c -o $(TEST)

clean:
	rm -f *.o *.so *.dll *.exe
	