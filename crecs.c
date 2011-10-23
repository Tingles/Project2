#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./setup.h"


/* local function prototype. */
static void pad(Record *);
static void write_record(FILE *, Record *);

Record * 
record_new(char *s){
  char * field;
  Record *r = malloc(sizeof (Record));
  
  field = strtok(s, ",");
  r->id = atoi(field);
  field = strtok(NULL,",");
  strncpy(r->countrycode, field, sizeof(r->countrycode));
  field = strtok(NULL,",");
  strncpy(r->name, field,sizeof(r->name));  
  field = strtok(NULL,",");
  strncpy(r->continent,field,sizeof(r->continent)); 
  field = strtok(NULL,",");
  strncpy(r->region,field,sizeof(r->region)); 
  field = strtok(NULL,",");
  r->surfacearea = atoi(field); 
  field = strtok(NULL,",");
  r->yearIndep = atoi(field); 
  field = strtok(NULL,",");
  r->population = atol(field); 
  field = strtok(NULL,",");
  r->lifexp = atoi(field); 
  field = strtok(NULL,",");
  r->gnp = atoi(field); 
  pad(r);
  return r;
}

static void
arraystorage_backup(void *self, FILE *f){
  if (self == 0)
    return;
  int i = 0;
  CountryStorage *rs = (CountryStorage *) self;

  for (i; i < rs->nrecords; i++)
    write_record(f,rs->countries[i]);
    
}

static void
arraystorage_init(void *self, void *val, int lim){
  if (self == 0)
    return;
  int i = 0;
  CountryStorage *rs = (CountryStorage *) self;

  rs->countries = (Recs *) malloc(sizeof (Recs) * lim);
  for (i; i < lim; i++){
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

Storage array_ops = {&arraystorage_backup,
		     &arraystorage_init,
		     &arraystorage_insert,
		     &arraystorage_destroy};

static void 
pad(Record *rec){
  int i;
  for(i = 0; i < sizeof (rec->name); i++)
    if (rec->name[i] == '\0')
      rec->name[i] = ' ';
  for(i = 0; i < sizeof (rec->continent); i++)
    if (rec->continent[i] == '\0')
      rec->continent[i] = ' ';
  for(i = 0; i < sizeof (rec->region); i++)
    if (rec->region[i] == '\0')
      rec->region[i] = ' ';
  return;
}

static void
write_record(FILE *f, Record * r){
  int i;

  if (r == 0){
    for(i = 0; i < sizeof (Record) - 3; i++)
      fputc(0,f);
  }
  else{
    fwrite(&(r->id), sizeof (r->id), 1, f);
    fwrite(&(r->countrycode), sizeof (r->countrycode), 1, f);
    fwrite(&(r->name), sizeof (r->name), 1, f);
    fwrite(&(r->continent), sizeof (r->continent), 1, f);
    fwrite(&(r->region), sizeof (r->region), 1, f);
    fwrite(&(r->surfacearea), sizeof (r->surfacearea), 1, f);
    fwrite(&(r->yearIndep), sizeof (r->yearIndep), 1, f);
    fwrite(&(r->population), sizeof (r->population), 1, f);
    fwrite(&(r->lifexp), sizeof (r->lifexp), 1, f);
    fwrite(&(r->gnp), sizeof (r->gnp), 1, f);
  }
}
