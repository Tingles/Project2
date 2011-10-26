#include "./setup.h"





Meta
readheader(FILE *f){
  struct meta header = {0,0,0};
  fread(&header.nrecords, sizeof(header.nrecords), 1, f);
  fread(&header.root, sizeof(header.root), 1, f);
  fread(&header.maxid, sizeof(header.maxid), 1, f);
  return header;
}

void
initialize_env(CountryStorage *cs, CodeIndex *ci, Meta *m){
  cs->nrecords = ci->n = m->nrecords;
  cs->maxid = m->maxid;
  ci->root = m->root;
  (cs->ops.init) (cs, (Record *) 0, cs->maxid);
  (ci->ops.init) (ci, 0, maxcountries);

  return;
}
