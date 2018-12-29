#ifndef _FILE_H_
#define _FILE_H_

typedef struct lligada {
    char name[3];
    char score[3];
    char date[8];
    struct lligada *next;
} *Users;

Users usersInit();

void addUser(Users users, char* name, char* date, char* score);

void freeUsers(Users users);

void write_to_file(Users users);

void read_from_file(Users users);

int scoreToInt(char* score);

char* scoreToChar(int valor);

#endif
