#NAME: Changyu Yan
#EMAIL: changyuyan@g.ucla.edu
#UID: 304566451


CC = gcc
CFLAGS = -Wall -Wextra -g -lmcrypt --save-temps
UID = 304566451
TARBALL = lab1b-$(UID).tar.gz
SOURCE =  Makefile README lab1b-client.c lab1b-server.c my.key


default:
	@$(CC) $(CFLAGS) -o lab1b-client lab1b-client.c
	@$(CC) $(CFLAGS) -o lab1b-server lab1b-server.c
client:
	@$(CC) $(CFLAGS) -o lab1b-client lab1b-client.c
server:
	@$(CC) $(CFLAGS) -o lab1b-server lab1b-server.c
clean:
	@rm -f lab1b-client lab1b-server *.tar.gz  *.s *.i *.o *.txt *.out
dist:
	@tar -czf $(TARBALL) $(SOURCE)