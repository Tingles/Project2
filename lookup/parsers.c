#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./setup.h"


static void pad(Record *);

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
  r->yearindep = atoi(field); 
  field = strtok(NULL,",");
  r->population = atol(field); 
  field = strtok(NULL,",");
  r->lifexp = atoi(field); 
  field = strtok(NULL,",");
  r->gnp = atoi(field); 
  pad(r);
  return r;
}

/*
 *transrecord_new takes a reference to string.  The function splits the string
 * at the first whitespace, replaceing the whitespace with a null character.
 * The function then returns a refernce to the beggining of the string.  The 
 * trick here is the argument is modified in the control loop.
 */
char *
parsetransrec(char *s){
  char *cp;

  for(cp = s; (*cp) != ' '; (*cp++)){             
  /* 
   *Tricky loop here. The dereference operator (*) 
   *  binds tighter than the inrement operator (++),
   *  therefor dereferecing what cp points to, ie the char,
   *  and then increments itself to the next char in line.
   *  NOTE: THIS MODIFIES THE ORIGINAL PARAMETER PASSED IN.
  */
    if (*cp == '\0'){
      fprintf(stderr, "invalid transaction.\n");
      abort();
    }
  }
  *cp = '\0';
  *cp++;
  return cp;
}

char *
strip(char *s){
  int i = 0;
  while(s[i] != '\r'){
    i++;
  }
  s[i] = '\0';
  return s;
}

ActionCode
action2enum(char *s){         /*goodness gracious this doubley nested case is ugly.*/
  ActionCode a;
  switch(s[0]){
  case 'Q': {  
    switch(s[1]){
    case 'N':
      a = Query_name;
      break;
    case 'I':
      a = Query_keyid;
      break;
    case 'C':
      a = Query_code;
      break;
    default :break;
    }
  }break;
  case 'L': {
    switch(s[1]){
    case 'N':
      a = List_name;
      break;
    case 'I':
      a = List_keyid;
      break;
    case 'C':
      a = List_code;
      break;
    default :break;
    }
  }break;
  default : 
    printf("Invalid transaction code.\n");
    a = -1;
  }
  return a;
}

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
