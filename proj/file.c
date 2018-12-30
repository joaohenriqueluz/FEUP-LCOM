#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <stdio.h>
#include "file.h"

Users usersInit(){

	Users users = malloc(sizeof(struct lligada));

	users->next = NULL;

	return users;
}

void addUser(Users users, char* name, char* date, char* score){
	Users l = users;
	Users pt, ant;
	pt = l;
	ant = malloc(sizeof(struct lligada));

	strcpy(name, ant->name);
	strcpy(score, ant->score);
	strcpy(date, ant->date);

	ant->next = NULL;

	if (l == NULL)
	{
		users = ant;
	}
	else{
		while( l->next != NULL && scoreToInt(l->next->score) > scoreToInt(score)){
			l = l->next;
		}
		l->next = ant;

		users = pt;
	}
}

void freeUsers(Users users){
	if (users == NULL)
	{
		return;
	}
	Users r = users;
	while(r != NULL){
		users = r;
		r = r->next;
		free(users);
	}
}

void write_to_file(Users users){
	if (users == NULL) return;

	Users l = users;

	FILE *ptr;
	char* file = "/home/lcom/labs/proj/users.txt";
	ptr = fopen(file, "w");
	
	while(l != NULL){
		int n = 0;
		while(n < 3){
			fprintf(ptr, "%c", users->name[n++]);
		}
		fprintf(ptr, "\t");
		n = 0;
		while(n < 3){
			fprintf(ptr, "%c", users->score[n++]);
		}
		fprintf(ptr, "\t");
		n = 0;
		while(n < 8){
			fprintf(ptr, "%c", users->date[n++]);
		}
		fprintf(ptr, "\n");
		l = l->next;
	}
}

void read_from_file(Users users){
	FILE *ptr;
	char* file = "/home/lcom/labs/proj/users.txt";
	ptr = fopen(file, "r");
	int count = 0;
	char c = fgetc(ptr);

	while(c != EOF){
		char name[3], score[3], date[8];
		count = 0;

		while(c != '\t'){
			name[count++] = c;
			c = fgetc(ptr);
		}
		c = fgetc(ptr);
		count = 0;
		while(c != '\t'){
			score[count++] = c;
			c = fgetc(ptr);
		}
		c = fgetc(ptr);
		count = 0;
		while(c != '\n'){
			date[count++] = c;
			c = fgetc(ptr);
		}
		c = fgetc(ptr);

		addUser(users,name,date,score);
	}
}

int scoreToInt(char* score){

	int valor = score[0]*100 + score[1]*10 + score[2];

	return valor;
}



