#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include "/home/adi/Downloads/sqlite-autoconf-3300100/sqlite3.h"
#define PORT 2909
int login(char *);
int removefile(char *);
void writefile(char *, char *);
extern int errno;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
//void raspunde(void *);

typedef struct
{
  pthread_t idThread; //id-ul thread-ului
  //int thCount;        //nr de conexiuni servite
} Thread;

Thread *threadsPool; //un array de structuri Thread

int sd;                                            //descriptorul de socket de ascultare
int nthreads;                                      //numarul de threaduri
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER; // variabila mutex ce va fi partajata de threaduri

void raspunde(int cl, int idThread);

int main(int argc, char *argv[])
{
  struct sockaddr_in server; // structura folosita de server
  void threadCreate(int);

  if (argc < 2)
  {
    fprintf(stderr, "Eroare: Primul argument este numarul de fire de executie...");
    exit(1);
  }
  nthreads = atoi(argv[1]);
  if (nthreads <= 0)
  {
    fprintf(stderr, "Eroare: Numar de fire invalid...");
    exit(1);
  }
  threadsPool = calloc(sizeof(Thread), nthreads);

  /* crearea unui socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[server]Eroare la socket().\n");
    return errno;
  }
  /* utilizarea optiunii SO_REUSEADDR */
  int on = 1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  /* pregatirea structurilor de date */
  bzero(&server, sizeof(server));

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
  server.sin_family = AF_INET;
  /* acceptam orice adresa */
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  /* utilizam un port utilizator */
  server.sin_port = htons(PORT);

  /* atasam socketul */
  if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[server]Eroare la bind().\n");
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen(sd, 2) == -1)
  {
    perror("[server]Eroare la listen().\n");
    return errno;
  }

  printf("Nr threaduri %d \n", nthreads);
  fflush(stdout);
  int i;
  for (i = 0; i < nthreads; i++)
    threadCreate(i);

  /* servim in mod concurent clientii...folosind thread-uri */
  for (;;)
  {
    printf("[server]Asteptam la portul %d...\n", PORT);
    pause();
  }
};

void threadCreate(int i)
{
  void *treat(void *);

  pthread_create(&threadsPool[i].idThread, NULL, &treat, (void *)i);
  return; /* threadul principal returneaza */
}
void *treat(void *arg)
{
  int client;

  struct sockaddr_in from;
  bzero(&from, sizeof(from));
  printf("[thread]- %d - pornit...\n", (int)arg);
  fflush(stdout);

  for (;;)
  {
    int length = sizeof(from);
    pthread_mutex_lock(&mlock);
    //printf("Thread %d trezit\n",(int)arg);
    if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
    {
      perror("[thread]Eroare la accept().\n");
    }
    pthread_mutex_unlock(&mlock);
    //threadsPool[(int)arg].thCount++;

    raspunde(client, (int)arg); //procesarea cererii
    /* am terminat cu acest client, inchidem conexiunea */
    close(client);
  }
}

void raspunde(int cl, int idThread)
{
  int i = 0;
  char s[1024];
  while (read(cl, &s, sizeof(s)) > 0)
  {
   // printf("%s\n", s);
    printf("[Thread %d]Mesajul a fost receptionat...%s\n", idThread, s);
   // printf("\n %s \n", s);
    printf("[Thread %d]Trimitem mesajul inapoi...%s\n", idThread, s);
    //---------------------------LOGIN----------------------------------------------------
    if (strcmp(s, "login") == 0)
    {
      if (write(cl, "Introduceti username-ul:", 25) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      if (read(cl, &s, sizeof(s)) > 0)
        if (login(s))
        {
          if (write(cl, "logged", 7) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost citit cu succes.\n", idThread);
        }
        else
        {
          if (write(cl, "denied", 8) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
        }
    }
    //-----------------------------------------------RECORD---------------------------------------------
    else if(strcmp(s, "inregistrare") == 0)
    {
      if (write(cl, "Introduceti username-ul:", 25) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      if (read(cl, &s, sizeof(s)) > 0)
        if (record(s))
        {
          if (write(cl, "succes", 7) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost citit cu succes.\n", idThread);
        }
        else
        {
          if (write(cl, "denied", 8) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
        }
    }
    //-----------------------------------------------LIST------------------------------------------------
    else if (strcmp(s, "list") == 0)
    {
      char page[1024], pages[1024] = "";
      struct dirent *de;
      DIR *dr = opendir(".");

      if (dr == NULL)
      {
        printf("Could not open current directory"); //write this to client
      }
      while ((de = readdir(dr)) != NULL)
      {
        if (strstr(de->d_name, ".html"))
          sprintf(pages + (strlen(pages)), "%s\n", de->d_name);
      }
      closedir(dr);
      if (write(cl, pages, sizeof(pages)) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
    //---------------------------------------------GET----------------------------------------------
    else if (strcmp(s, "get") == 0)
    {
      if (write(cl, "Introduceti numele paginii:", 28) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      if (read(cl, &s, sizeof(s)) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la read() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul %s a fost citit cu succes.\n", idThread, s);
      if (strstr(s, ".html"))
      {
        if (access(s, F_OK) != -1) //verificam daca exista
        {
          //deschidem fisierul si trimitem datele
          char name[1024];
          strcpy(name, s);
          if (write(cl, "Unde ati vrea sa fie salvat(calea):", 36) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul Unde ati vrea sa fie salvat(calea): a fost trasmis cu succes.\n", idThread);
          FILE *fp2;
          char content[1024] = "", character;
          if (read(cl, &s, 3) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la read() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost citit cu succes.\n", idThread);
          if (strcmp(s, "ok") == 0)
          {
            int size1;
            fp2 = fopen(name, "r");
            struct stat st;
            stat(name, &st);
            size1 = st.st_size;
            //write number of bytes to client
            printf("Dimensiune: %d biti.\n",size1);
            if (write(cl, &size1, sizeof(int)) <= 0)
            {
              printf("[Thread %d] ", idThread);
              perror("[Thread]Eroare la write() catre client.\n");
            }
            else
              printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
            while ((character = fgetc(fp2)) != EOF) //citeste caracter cu caracter si trimite catre client
            {
              sprintf(content + (strlen(content)), "%c", character);
              if (strlen(content) == 1023)
              {
                if (write(cl, content, sizeof(content)) <= 0)
                {
                  printf("[Thread %d] ", idThread);
                  perror("[Thread]Eroare la write() catre client.\n");
                }
                else
                  printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
                strcpy(content, "");
              }
            }
            if (write(cl, content, sizeof(content)) <= 0)
            {
              printf("[Thread %d] ", idThread);
              perror("[Thread]Eroare la write() catre client.\n");
            }
            else
              printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
            fclose(fp2);
            if (read(cl, &s, 7) <= 0)
            {
              printf("[Thread %d] ", idThread);
              perror("[Thread]Eroare la read() catre client.\n");
            }
            else
              printf("[Thread %d]Mesajul a fost citit cu succes.\n", idThread);
            if (write(cl, "succes", 7) <= 0)
            {
              printf("[Thread %d] ", idThread);
              perror("[Thread]Eroare la write() catre client.\n");
            }
            else
              printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
          }
          else
          {
            if (write(cl, "err..bad path!", 15) <= 0)
            {
              printf("[Thread %d] ", idThread);
              perror("[Thread]Eroare la write() catre client.\n");
            }
            else
              printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
          }
        }
        else
        {
          // returnam fisieul nu exista
          if (write(cl, "Fisierul nu exista!", 20) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
        }
      }
      else
      {
        if (write(cl, "Fisierul nu exista!", 20) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
        if (read(cl, &s, 11) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la read() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost citit cu succes.\n", idThread);

        if (write(cl, "Fisierul nu exista!", 20) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      }
    }

    //--------------------------------------ADD---------------------------------------------------
    else if (strcmp(s, "add") == 0)
    {
      char name[512];
      if (read(cl, &s, sizeof(s)) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la read() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost primit cu succes.\n", idThread);
      strcpy(name, s);
      if (access(s, F_OK) != -1) //verificam daca exista
      {
        if (write(cl, "Fisierul deja exista.", 22) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      }
      else
      {
        if (write(cl, "Nu exista niciun fisier cu acest nume pe server.", 49) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      }
      if (read(cl, &s, sizeof(s)) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la read() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost primit cu succes.\n", idThread);
      if (strcmp(s, "da") == 0)
      {
        if (read(cl, &s, sizeof(s)) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la read() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost primit cu succes.\n", idThread);
        if (strcmp("ok", s) == 0)
        {
          removefile(name);
          FILE *fp3;
          int size1;
          if (fp3 = fopen(name, "w"))
            fclose(fp3);
          if (read(cl, &size1, sizeof(int)) <= 0)
          {
            printf("[Thread %d]\n", idThread);
            perror("Eroare la read() de la client.\n");
          }
          printf("Dimensiune: %d biti.\n", size1);
          while ((size1 - 1023) > 0)
          {
            if (read(cl, &s, sizeof(s)) <= 0)
            {
              printf("[Thread %d] ", idThread);
              perror("[Thread]Eroare la read() catre client.\n");
            }
            else
              printf("[Thread %d]Mesajul a fost primit cu succes.\n", idThread);
            writefile(name, s);
            size1 = size1 - 1023;
          }
          if (read(cl, &s, sizeof(s)) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la read() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost primit cu succes.\n", idThread);
          //printf("\n%s\n", s);
          writefile(name, s);
          if (write(cl, "succes", 7) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
        }
        else
        {
          if (write(cl, "Fisierul nu exista/Calea introdusa nu este corecta!", 52) <= 0)
          {
            printf("[Thread %d] ", idThread);
            perror("[Thread]Eroare la write() catre client.\n");
          }
          else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
        }
      }
      else
      {
        if (write(cl, "ok", 3) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      }
    }
    //-------------------------------------REMOVE----------------------------------------------------
    else if (strcmp(s, "remove") == 0)
    {
      if (write(cl, "Introduceti numele paginii:", 28) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      if (read(cl, &s, sizeof(s)) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la read() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost primit cu succes.\n", idThread);
      if (removefile(s))
        if (write(cl, "removed", 8) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      else
      {
        if (write(cl, "file does not exist", 20) <= 0)
        {
          printf("[Thread %d] ", idThread);
          perror("[Thread]Eroare la write() catre client.\n");
        }
        else
          printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
      }
    }
    //--------------------------------------not loging--------------------------------------------------
    else if (strcmp(s, "notlogin") == 0)
    {
      if (write(cl, "Nu te-ai logat!", 16) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
    //-----------------------------------------help---------------------------------------------
    else if (strcmp(s, "help") == 0)
    {
      if (write(cl, "\n\n1.list(poti vedea toate paginile html care exista pe server)\n2.get(poti extrage o pagina html)\n3.add(poti dauga o pagina html)\n4.remove(poti sterge o pagina html)\n5.logout(te poti deloga si loga cu alt username)\n6.help(listarea comenzilor)\n7.exit(Tap for surprise.)\n\n", 270) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
    //----------------------------------------exit------------------------------------------------------
    else if (strcmp(s, "exit") == 0)
    {
      if (write(cl, "La revedere!", 13) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
    //------------------------------------------logout--------------------------------------------------
    else if (strcmp(s, "logout") == 0)
    {
      if (write(cl, "V-ati delogat!", 15) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
    //-----------------------------------anything else---------------------------------------------------
    else if (strcmp(s, "list") != 0 && strcmp(s, "login") != 0 && strcmp(s, "add") != 0 && strcmp(s, "get") != 0)
    {
      if (write(cl, "Nu este o comanda valida!", 26) <= 0)
      {
        printf("[Thread %d] ", idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      else
        printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", idThread);
    }
  }
}
char name_global[100];
static int my_callback1(void *data, int argc, char **argv, char **azColName)
{
  int i;
  for (i = 0; i < argc; i++)
  {
    if (strcmp(name_global, argv[i]) == 0)
    {
      strcpy(name_global, "ok");
    }
  }
  return 0;
}
int login(char name[1024])
{
  sqlite3 *db;
  char *ErrMsj = 0;
  int errcatch;
  char *sql;
  char *data;
  strcpy(name_global, name);
  printf("%s", name_global);
  /* Open database */
  errcatch = sqlite3_open("/home/adi/Desktop/proiectRMutex/database.db", &db);

  if (errcatch)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return (0);
  }
  sql = "SELECT * from USERS";

  errcatch = sqlite3_exec(db, sql, my_callback1, (void *)data, &ErrMsj);
  if (errcatch != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", ErrMsj);
    sqlite3_free(ErrMsj);
  }
  sqlite3_close(db);
  if (strcmp(name_global, "ok") == 0)
    return 1;

  return 0;
}
static int my_callback2(void *Unused, int argc, char **argv, char **Column) {
   return 0;
}

int record(char name[1024]) {
   sqlite3 *db;
   char *ErrMsj = 0;
   int rc;
   char sql[1024];
   /* Open database */
   rc = sqlite3_open("/home/adi/Desktop/proiectRMutex/database.db", &db);
   sql[0]=0;
   if( rc ) {

      return 0;
   }

   strcpy(sql,"");
   if(login(name))
   {
     return 0;
   }
   else
   sprintf(sql,"INSERT INTO USERS (USERNAME) VALUES ('%s'); ",name);
   rc = sqlite3_exec(db, sql, my_callback2, 0, &ErrMsj);
   
   if( rc != SQLITE_OK ){
      return 0;
   }
   sqlite3_close(db);
   return 1;
}
int removefile(char name[1024])
{
  if (strstr(name, ".html"))
    if (remove(name) == 0)
      return 1;
  return 0;
}
void writefile(char *path, char *mess)
{
  FILE *filedesc;
  filedesc = fopen(path, "a");
  fprintf(filedesc, "%s", mess);
  fclose(filedesc);
}
