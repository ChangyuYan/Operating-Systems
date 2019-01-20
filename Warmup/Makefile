# Name: Changyu Yan
# UID: 304-566-451
# Email: changyuyan@g.ucla.edu

CC = gcc
CFLAGS = -Wall -Wextra -g
UID = 304566451
TARBALL = lab0-$(UID).tar.gz
SOURCE = lab0.c Makefile README breakpoint.png backtrace.png

default:
	@$(CC) $(CFLAGS) -o lab0 lab0.c

clean:
	@rm -f lab0 *.tar.gz *.o *.txt a.out
	
dist: default
	tar -czf $(TARBALL) $(SOURCE)

check: clean default smokeTest
	@echo "********PASSED ALL TEST CASES!********"
	
smokeTest: 
	@./lab0 --segfault --catch > a.out 2>&1 /dev/null; \
	if [ $$? -ne 4 ]; then \
	  echo "ERROR: RETURN SHOULD BE 4 FOR --catch"; \
	  exit 1; \
	fi
	@echo "Hello World" > new.txt; \
	./lab0 --input=new.txt --output=nnew.txt; \
	if [ $$? -ne 0 ]; then \
	  echo "ERROR: RETURN SHOULD BE 0"; \
	  exit 1; \
	fi
	@$(diff new.txt nnew.txt) > DIFF.txt; \
	if [ -s DIFF.txt ]; then \
	  echo "ERROR: Input and Output files are not the same!"; \
	  exit 1; \
	fi
	@rm -f new.txt nnew.txt; \
	./lab0 --input=null.txt --output=nnew.txt > a.out 2>&1 /dev/null; \
	if [ $$? -ne 2 ]; then \
	  echo "ERROR: RETURN SHOULD BE 2"; \
	  exit 1; \
	fi
	@echo "Hello World" > new.txt; \
	touch nnew.txt; \
	chmod -w nnew.txt; \
	./lab0 --input=new.txt --output=nnew.txt > a.out 2>&1 /dev/null; \
	if [ $$? -ne 3 ]; then \
	  echo "ERROR: RETURN SHOULD BE 3"; \
	  exit 1; \
	fi