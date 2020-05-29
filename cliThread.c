#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
extern int errno;
void writefile(char *path, char *mess)
{
  FILE *filedesc;
  filedesc = fopen(path, "a");
  fprintf(filedesc, "%s", mess);
  fclose(filedesc);
}
int port;
int recordfunction(int sd)
{
  char message[1024];
  if (read(sd, &message, 28) < 0)
  {
    perror("[client]Eroare la read() de la server.\n");
    return errno;
  }
  else
  {
    printf("%s", message);
  }
  scanf("%s", message);
  if (write(sd, &message, sizeof(message)) <= 0)
  {
    perror("[client]Eroare la write() spre server.\n");
    return errno;
  }
  return 1;
}
int loginfunction(int sd)
{
  char message[1024];
  if (read(sd, &message, 28) < 0)
  {
    perror("[client]Eroare la read() de la server.\n");
    return errno;
  }
  else
  {
    printf("%s", message);
  }
  scanf("%s", message);
  if (write(sd, &message, sizeof(message)) <= 0)
  {
    perror("[client]Eroare la write() spre server.\n");
    return errno;
  }
  return 1;
}
int removefunction(int sd)
{
  char message[1024];
  if (read(sd, &message, 28) < 0)
  {
    perror("[client]Eroare la read() de la server.\n");
    return errno;
  }
  else
  {
    printf("%s", message);
  }
  scanf("%s", message);
  if (write(sd, &message, sizeof(message)) <= 0)
  {
    perror("[client]Eroare la write() spre server.\n");
    return errno;
  }
  return 1;
}
int getfunction(int sd)
{
  char path[1024], name[512], message[1024];
  int size1;
  if (read(sd, &message, 28) < 0)
  {
    perror("[client]Eroare la read() de la server.\n");
    return errno;
  }
  else
  {
    if (strcmp(message, "Nu esti logat!") == 0)
    {
      if (write(sd, &name, sizeof(name)) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
    }
    else
      printf("%s", message);
  }
  scanf("%s", name);
  if (write(sd, &name, sizeof(name)) <= 0)
  {
    perror("[client]Eroare la write() spre server.\n");
    return errno;
  }
  if (read(sd, &message, 36) < 0)
  {
    perror("[client]Eroare la read() de la server.\n");
    return errno;
  }
  else
  {
    if (strcmp(message, "Fisierul nu exista!") == 0)
      if (write(sd, "Nu exista!", 11) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
      else
      {

        return 0;
      }
    printf("%s", message);
    scanf("%s", path);
    sprintf(path + strlen(path), "/%s", name);
    FILE *fp1;
    if (fp1 = fopen(path, "w+"))
    {
      if (write(sd, "ok", 3) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
      //read number of bytes to read from server
      if (read(sd, &size1, sizeof(int)) < 0)
      {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
      }
      close(fp1);
      printf("\nDimesiune: %d biti.", size1);
      while ((size1 - 1023) > 0)
      {
        if (read(sd, &message, sizeof(message)) < 0)
        {
          perror("[client]Eroare la read() de la server.\n");
          return errno;
        }
        writefile(path, message);
        size1 = size1 - 1023;
      }
      if (read(sd, &message, sizeof(message)) < 0)
      {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
      }
      writefile(path, message);
      if (write(sd, "succes", 7) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
    }
    else
    {
      if (write(sd, "nu", 3) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
      else
      {
        return 0;
      }
    }
  }
  return 0;
}
int addfunction(int sd)
{
  char name[512], path[1024], message[1024];
  printf("Introduceti numele fisierului:");
  scanf("%s", name);
  if (write(sd, &name, sizeof(name)) <= 0)
  {
    perror("[client]Eroare la write() spre server.\n");
    return errno;
  }
  if (read(sd, &message, 49) < 0)
  {
    perror("[client]Eroare la read() de la server.\n");
    return errno;
  }
  else
  {
    printf("%s\n", message);
  }
  if (strcmp(message, "Fisierul deja exista.") == 0)
  {
    printf("WARNING:Daca adaugati acest fisier pe server, veti suprascrie fisierul deja existent!\n");
  }
  else
  {
    printf("WARNING:Daca fisierul adaugat nu este de tipul html, acesta va fi sters ulterior!\n");
  }
  printf("Doriti sa continuati?\n (Tastati da/nu):");
  scanf("%s", message);
  if (write(sd, &message, 3) <= 0)
  {
    perror("[client]Eroare la write() spre server.\n");
    return errno;
  }
  if (strcmp(message, "da") == 0)
  {
    printf("Introduceti calea exacta catre fisier:");
    scanf("%s", path);
    sprintf(path + strlen(path), "/%s", name);
    char content[1024] = "";
    FILE *fp4;
    fp4 = fopen(path, "r");
    if (fp4 == NULL)
    {
      if (write(sd, "nu", 3) <= 0)
      {
        perror("[client]Eroare la write()1 spre server.\n");
        return errno;
      }
      return 0;
    }
    else
    {
      if (write(sd, "ok", 3) <= 0)
      {
        perror("[client]Eroare la write()2 spre server.\n");
        return errno;
      }
    }
    char character;
    int size1;
    struct stat st;
    stat(path, &st);
    size1 = st.st_size;
    printf("\nDimensiune: %d biti.", size1);
    if (write(sd, &size1, sizeof(int)) <= 0)
    {
      perror("[client]Eroare la write() spre server.\n");
      return errno;
    }
    while ((character = fgetc(fp4)) != EOF)
    {
      sprintf(content + (strlen(content)), "%c", character);
      if (strlen(content) == 1023)
      {
        if (write(sd, &content, sizeof(content)) <= 0)
        {
          perror("[client]Eroare la write() spre server.\n");
          return errno;
        }
        strcpy(content, "");
      }
    }
    if (write(sd, &content, sizeof(content)) <= 0)
    {
      perror("[client]Eroare la write() spre server.\n");
      return errno;
    }
    fclose(fp4);
    return 1;
  }
  else
  {
    return 0;
  }
}

int main(int argc, char *argv[])
{
  int sd;                    // descriptorul de socket
  struct sockaddr_in server; // structura folosita pentru conectare
                             // mesajul trimis
  char buf[1024];
  if (argc != 3)
  {
    printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }
  port = atoi(argv[2]);
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Eroare la socket().\n");
    return errno;
  }
  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;//internet domain
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons(port);

  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[client]Eroare la connect().\n");
    return errno;
  }
  char login[100], exit1[100], logout[100];
  strcpy(buf, "start");
  strcpy(logout, "start");
  strcpy(login, "start");
  strcpy(exit1, "start");
  printf("\e[1;1H\e[2J");//just this line from here: https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c/36253316
  //it clears the screen before starting my project
  printf("                                  BUN VENIT!                \n\n");
  printf("Inainte sa puteti executa orice comanda trebuie sa va logati!\n\n");
  printf("(ATENTIE!!)Daca nu aveti un cont, va puteti inregistra tastand 'inregistrare'!\n\n");
  printf("Orice alta comanda nu va fi considerata valida pana cand nu va logati!\n\n");
  printf("Dupa ce v-ati logat tastati 'help' pentru a vedea ce comenzi sunt disponibile!\n\n");
  printf("Pentru a iesi din program tastati 'exit'!\n\n");
  while (strcmp(exit1, "exit") != 0)
  {
    while (strcmp(login, "login") != 0)
    {

      printf("\nIntroduceti o comanda: ");
      scanf("%s", buf);
      strcpy(login, buf);
      if (strcmp(buf, "login") != 0 && strcmp(buf, "exit") != 0 && strcmp(buf, "inregistrare") != 0)
      {
        strcpy(buf, "nothing");
        printf("Trebuie sa te loghezi!");
      }
      if (write(sd, &buf, sizeof(buf)) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
      if (strcmp(buf, "login") == 0)
      {
        int var = loginfunction(sd);
      }
      else
      {
        if (strcmp(buf, "inregistrare") == 0)
        {
          int var = recordfunction(sd);
        }
        else
          strcpy(buf, "notlogin");
      }
      if (read(sd, &buf, sizeof(buf)) < 0)
      {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
      }
      printf("\n%s\n", buf);
      if (strcmp(buf, "denied") == 0)
        strcpy(login, "start");
      else if (strcmp(buf, "La revedere!") == 0)
      {
        strcpy(exit1, "exit");
        strcpy(login, "login");
        strcpy(logout, "logout");
      }
    }
    while (strcmp(logout, "logout") != 0)
    {
      printf("\nIntroduceti o comanda: ");
      fflush(stdout);
      scanf("%s", buf);
      strcpy(logout, buf);

      if (strcmp(buf, "login") == 0)
        strcpy(buf, "nothing");
      if (write(sd, &buf, sizeof(buf)) <= 0)
      {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
      }
      if (strcmp(buf, "get") == 0)
      {
        int var = getfunction(sd);
      }
      if (strcmp(buf, "add") == 0)
      {
        int var = addfunction(sd);
      }
      if (strcmp(buf, "remove") == 0)
      {
        int var = removefunction(sd);
      }
      if (read(sd, &buf, sizeof(buf)) < 0)
      {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
      }
      if (strcmp(logout, "exit") == 0)
      {
        strcpy(exit1, "exit");
        strcpy(logout, "logout");
      }
      printf("\n%s\n", buf);
    }
    strcpy(login, "start");
    strcpy(logout, "start");
  }
  close(sd);
  return 0;
}
