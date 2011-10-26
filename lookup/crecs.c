#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "./setup.h"


/* local function prototype. */

static void write_record(FILE *, Record *);
static Record * read_record(FILE *);
static void convertToUpperCase(char *);
static void prettyprint(Record *);

static void
arraystorage_backup(void *self, FILE *f){
  if (self == 0)
    return;
  int i;
  CountryStorage *rs = (CountryStorage *) self;

  for (i=0; i < rs->nrecords; i++)
    write_record(f,rs->countries[i]);
}

static void
arraystorage_load(void *self, FILE *f){
  if (self == 0)
    return;
  int i;
  CountryStorage *rs = (CountryStorage *) self;

  for (i = 0; i < rs->maxid; i++){
    rs->countries[i] = (Recs) read_record(f);
  }
}

static void
arraystorage_init(void *self, void *val, int lim){
  if (self == 0)
    return;
  int i;
  CountryStorage *rs = (CountryStorage *) self;

  rs->countries = (Recs *) malloc(sizeof (Recs) * lim);
  for (i=0; i < lim; i++){
    rs->countries[i] = (Recs) val;
  }
  rs->nrecords = 0;
}

static void 
arraystorage_insert(void *self, void *data){
  if (self == 0)
    return;
  CountryStorage *rs = (CountryStorage *) self;
  Record *r = (Record *) data;
  
  rs->countries[r->id] = r;
  if (r->id > rs->maxid)
    rs->maxid = r->id;
  rs->nrecords++;
}

static void
arraystorage_destroy(void *self){
  if (self == 0)
    return;
  CountryStorage *rs = (CountryStorage *) self;
  free(rs->countries);
}

static int 
arraystorage_query(void *self, void *mode, void *target){
  int outcome = -1;
  if (self == 0)
    return -1;
  CountryStorage *rs = (CountryStorage *) self;
  ActionCode AC  = *((int *) mode);
  if (rs->ops.t_helper[AC])
    outcome = (rs->ops.t_helper[AC])(rs, target);
  return outcome;
}

static int
array_queryname(void *self, void *target){
  int i,ncompares;
  size_t length;
  int result = -1;
  
  if (self == 0 || target == 0)
    return result;
  CountryStorage *rs = (CountryStorage *) self;
  
  for (ncompares = i = 0; i < rs->maxid; i++){
    if(rs->countries[i] != 0){
      length = sizeof (rs->countries[i]->name);
      int cond,diff;
      char *current = strndup(rs->countries[i]->name, length);
      char *pt = strdup(current);
      strncpy(pt, (char *)target, length);
      if ((diff = length - strlen(pt)) > 0)
	memset((void *)(pt + (length - diff)), (unsigned char) ' ', diff);
      convertToUpperCase(current);
      convertToUpperCase(pt);
      cond = strncmp(current, pt, length);
      free(current); free(pt);
      if (cond == 0){
	prettyprint(rs->countries[i]);
	return ++ncompares;
      }
    }
    ncompares++;
  }
  printf("NAME: %s.\n", (char *) target);
  return ncompares;
}

static int
array_queryid(void *self, void *target){
  int ncompares = 0;
  int result = -1;
  
  if (self == 0 || target == 0)
    return result;
  CountryStorage *rs = (CountryStorage *) self;  
  int tid = atoi((char *) target);

  if(rs->countries[tid]){
    prettyprint(rs->countries[tid]);
    return ++ncompares;
  }
  else{
    printf("ID: %d.\n", tid);
  }
  return ++ncompares;
}

static int 
array_listid(void *self, void *target){
  return 0;  
}

Storage array_ops = {&arraystorage_backup,
		     &arraystorage_load,
		     &arraystorage_init,
		     &arraystorage_insert,
		     &arraystorage_destroy,
                     &arraystorage_query,
		     {&array_queryname,
		      &array_queryid,
		      NULL,
		      &array_listid,
		      NULL,
		      NULL}
};

static void
write_record(FILE *f, Record * r){
  int i;
  Offset offset = alignment_offset;/*need offset because shorts are size word in memory.*/
  if (r == 0){
    for(i = 0; i < sizeof (Record) - offset  ; i++)
      fputc(0,f);
  }
  else{
    fwrite(&(r->id), sizeof (r->id), 1, f);
    fwrite(&(r->countrycode), sizeof (r->countrycode), 1, f);
    fwrite(&(r->name), sizeof (r->name), 1, f);
    fwrite(&(r->continent), sizeof (r->continent), 1, f);
    fwrite(&(r->region), sizeof (r->region), 1, f);
    fwrite(&(r->surfacearea), sizeof (r->surfacearea), 1, f);
    fwrite(&(r->yearindep), sizeof (r->yearindep), 1, f);
    fwrite(&(r->population), sizeof (r->population), 1, f);
    fwrite(&(r->lifexp), sizeof (r->lifexp), 1, f);
    fwrite(&(r->gnp), sizeof (r->gnp), 1, f);
  }
}

static Record *
read_record(FILE *f){
  Record *r;
  Offset offset = alignment_offset;  /*same reason as above...*/
  r = malloc(sizeof (Record));
  if (r == NULL){
    printf("%m");
    return NULL;
  } 
  /* peek at id, in the case it is 0, waste 61 bytes of input file and return null. */
  fread(&(r->id), sizeof (r->id), 1, f);
  if (r->id == 0){
    int i = 0;
    while (i < sizeof (Record) - sizeof (r->id) - offset){
      fgetc(f);
      i++;
    }
    return NULL;
  }
  fread(&(r->countrycode), sizeof (r->countrycode), 1, f);
  fread(&(r->name), sizeof(r->name), 1, f);
  fread(&(r->continent), sizeof(r->continent), 1, f);
  fread(&(r->region), sizeof(r->region), 1, f);
  fread(&(r->surfacearea), sizeof(r->surfacearea), 1, f);
  fread(&(r->yearindep), sizeof(r->yearindep), 1, f);
  fread(&(r->population), sizeof(r->population), 1, f);
  fread(&(r->lifexp), sizeof(r->lifexp), 1, f);
  fread(&(r->gnp), sizeof(r->gnp), 1, f);
  return r;
}

static void
convertToUpperCase(char *s){
    do {
      *s = toupper((unsigned char) *s);
    } while(*s++ != '\0');
}

static void
prettyprint(Record *r){
  printf("%03d %3s %17s %11s %10s |%0d|%'4d|%d|%f|%d\n", r->id, r->countrycode, r->name, r->continent, r->region, r->yearindep, r->population, r->lifexp, r->gnp);
  return;
}

