CC=gcc
CFLAGS= -Wall -O -c
JSMN= /home/dvddaver/Documents/dissertation/src/parser/jsmn/libjsmn.a


install: install-info Install-libraries ClusTheDroid clean-up

install-info:
ifneq ("$(wildcard $(JSMN))", "")
	@echo "[+] Installing [+]"
	@echo "libjsmn.a found"
else
	$(error libjsmn.a does not exist. Please install and set variable)
endif	

ClusTheDroid: hash.o priq.o parser.o main.o hierarchical.o
	@echo "[+] Libraries installed successfully [+]"
	$(CC) -Wall main.o hierarchical.o parser.o priq.o hash.o $(JSMN) -o ClusTheDroid
	@echo "[+] Installation successfully installed [+]"


Install-libraries: hash.o priq.o parser.o clusthedroid.o hierarchical.o

hierarchical.o:
	$(CC) $(CFLAGS) hierarchical.c

main.o:
	$(CC) $(CFLAGS) clusthedroid.c

parser.o:
	$(CC) $(CFLAGS) parser.c

priq.o:
	$(CC) $(CFLAGS) priq.c

hash.o:
	$(CC) $(CFLAGS) hash.c

clean-up:
	@echo "[+] Cleaning up [+]"
	rm -rf *o
