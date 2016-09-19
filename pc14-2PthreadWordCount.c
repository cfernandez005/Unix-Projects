//Assignment PC14-2: Handling Several Files
//Chris Fernandez
//Editor: Notepad++
//Compiler: gcc
//Date: 3/9/15
//CS 3340 - Winter 2015

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "ctype.h"

struct arg_set
{
  char* fname;
  int count;
};

struct arg_set* mailbox;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;

void* count_words(void*);

int main( int ac, char* av[] )
{   
  if (ac < 2)
  {
    printf("\n::pc14-2 Command::\n");
		printf("  ./pc14-2 File1, File2,.. : Enter one of more filenames\n");
		printf("                             Returns the total amount of\n");
		printf("                             words in each file\n\n");
    exit(1);
  }
	
  int reports_in = 0;
  int total_words = 0;
  pthread_mutex_lock( &lock );
	
	pthread_t t[ac-1];
	struct arg_set args[ac-1];
	int i;
	for (i = 0; i < (ac-1); i++)
	{
	  args[i].fname = av[i+1];
		args[i].count = 0;
		pthread_create(&t[i], NULL, count_words, (void*) &args[i]);
	}
	
  printf("\n");
  while (reports_in < (ac-1))
  {
    printf("MAIN:  waiting for flag to go up\n");
    pthread_cond_wait(&flag, &lock);
    printf("MAIN:  Wow! flag was raised, I have the lock\n");
    printf("%10d: %s\n", mailbox->count, mailbox->fname);
    total_words += mailbox->count;
    
		for (i = 0; i < (ac-1); i++)
		  if (mailbox == &args[i])
			  pthread_join(t[i], NULL);

    mailbox = NULL;
    pthread_cond_signal(&flag);
    reports_in++;     
  }

  printf("%10d: totalwords\n\n", total_words);
}

void* count_words(void* a)
{
  struct arg_set* args = a;
  FILE* fp;
  int c, prevc = '\0';
    
  if ( (fp = fopen(args->fname, "r")) != NULL )
  {
    while ( (c = getc(fp)) != EOF )
    {
      if (!isalnum(c) && isalnum(prevc))
        args->count++;
      prevc = c;
    }
    fclose(fp);
  }
  else
    perror(args->fname);

  printf("COUNT: waiting to get lock\n");
  pthread_mutex_lock(&lock);
  printf("COUNT: have the lock, storing data\n");
    
  if (mailbox != NULL)
    pthread_cond_wait(&flag, &lock);
        
  mailbox = args;
  printf("COUNT: raising flag\n");
  pthread_cond_signal(&flag);
  pthread_mutex_unlock(&lock);
        
  return NULL;
}
