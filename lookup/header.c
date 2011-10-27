#include <stdio.h>
#include <string.h>
#include "./setup.h"

Meta
readheader(FILE *f){
  struct meta header = {0,0,0};
  fread(&header.nrecords, sizeof(header.nrecords), 1, f);
  fread(&header.root, sizeof(header.root), 1, f);
  fread(&header.maxid, sizeof(header.maxid), 1, f);
  return header;
}
/*********************************************************************************/
void
initialize_env(CountryStorage *cs, CodeIndex *ci, Meta *m){
  (ci->ops.init) (ci, 0, maxcountries);
  (cs->ops.init) (cs, (Record *) 0, m->maxid);
  (cs->nrecords) = (ci->n) = (m->nrecords);
  cs->maxid = m->maxid;
  ci->root = m->root;
  return;
}
/*********************************************************************************/
void
prettyprint(Record *r){
  size_t length;
  char *code, *name, *cont, *region;
  length = sizeof(r->countrycode);
  code = strndup(r->countrycode, length);

  length = sizeof(r->name);
  name = strndup(r->name, length);

  length = sizeof(r->continent);
  cont = strndup(r->continent, length);

  length = sizeof(r->region);
  region = strndup(r->region, length);

  printf("%03hd %s %s %s %s |%'10.d|%'6.d|%'13.ld|%8.4f|%'7.d\n",r->id, code, name, cont, region, r->surfacearea, r->yearindep, r->population, r->lifexp, r->gnp);

  return;
}
/*********************************************************************************/
/*********************************************************************************/
