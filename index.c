#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./setup.h"

/* local function proto */
static void write_indice(FILE *, BstNode);
static void new_indice(CodeIndex *, Record *, unsigned int);

static void
bstindex_backup(void *self, FILE *f){
  if (self == 0)
    return;
  int i = 0;
  CodeIndex *rs = (CodeIndex *) self;
  
  for (i; i < rs->n; i++)
    write_indice(f, rs->countrycodes[i]);
}

static void
bstindex_init(void *self, void *val, int lim){
  if (self == 0)
    return;
  CodeIndex *rs = (CodeIndex *) self;
  short int *si = (signed short *) val;
  
  rs->countrycodes = (BST) malloc(sizeof (BstNode) * lim);
  rs->countrycodes[0]._lcp = rs->countrycodes[0]._rcp = -1;
  rs->root = *si;
  rs->n = -1;
  
}

static void
bstindex_insert(void *self, void *data){
  int cond, i , parent;
  char dir; 
  cond = i = parent = 0;

  if (self == 0)
    return;
  CodeIndex *rs = (CodeIndex *) self;
  Record *r = (Record *) data;

  if (rs->n == -1){  /* signifies an emptry tree. */
    rs->n = 0;
    new_indice(rs, r, rs->n);
  }
  else{              /* normal case. */
    i = rs->n;
    while (i != -1){
      parent = i;
      if (cond = strncmp(r->countrycode, rs->countrycodes[parent]._keyid, 3) == 0){
	printf("code already exists in the index.\n");
	return;
      }
      else if (cond < 1){
	i = rs->countrycodes[i]._lcp;
	dir = 'l';
      }
      else{
	i = rs->countrycodes[i]._rcp;
	dir = 'r';
      }
    }
    if (dir == 'l'){
      rs->countrycodes[parent]._lcp = rs->n;
      new_indice(rs,r,rs->n);
    }
    else{
      rs->countrycodes[parent]._rcp = rs->n;
      new_indice(rs,r,rs->n);
    }
  }
}

static void 
bstindex_destroy(void *self){
  if (self == 0)
    return;

  CodeIndex *rs = (CodeIndex *) self;
  free(rs->countrycodes);
}

Storage bst_ops = {&bstindex_backup,
		   &bstindex_init,
		   &bstindex_insert,
		   &bstindex_destroy};

static void 
write_indice(FILE *f, BstNode n){
  BstNode *p;
  p = &n;
  fwrite(p, sizeof (BstNode), 1, f);
}		     

static void
new_indice(CodeIndex *i, Record *r, unsigned int n){
  if (i != 0 && r != 0){
    i->countrycodes[n]._keyid = strndup(r->countrycode, sizeof (r->countrycode));
    i->countrycodes[n]._lcp = i->countrycodes[n]._rcp = -1;
    i->n++;
  }
}
