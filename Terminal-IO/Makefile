#NAME: Changyu Yan
#EMAIL: changyuyan@g.ucla.edu
#UID: 304566451


CC = gcc
CFLAGS = -Wall -Wextra -g
UID = 304566451
TARBALL = lab1a-$(UID).tar.gz
SOURCE = lab1a.c Makefile README 

default:
	@$(CC) $(CFLAGS) -o lab1a lab1a.c
clean:
	@rm -f lab1a *.tar.gz *.o *.txt *.out
dist: default
	tar -czf $(TARBALL) $(SOURCE)