#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./setup.h"

static char *ns = {"\0\0\0"};
/* local function proto */
static void write_indice(FILE *, BstNode);
static BstNode read_indice(FILE *);
static void new_indice(CodeIndex *, Record *, unsigned int);

/*
 * local function definition.
 */
static void
bstindex_backup(void *self, FILE *f){
  if (self == 0)
    return;
  int i;
  CodeIndex *rs = (CodeIndex *) self;
  
  for (i = 0; i < rs->n; i++)
    write_indice(f, rs->countrycodes[i]);
}
/*********************************************************************************
 * Load a bst from file. */
static void
bstindex_load(void *self, FILE *f){
  if (self == 0)
    return;
  int i;
  CodeIndex *rs = (CodeIndex *) self;
  for (i = 0; i < rs->n; i++)
    rs->countrycodes[i] = read_indice(f);
}
/*********************************************************************************
 * Allocate storage space for Index. */
static void
bstindex_init(void *self, void *val, int lim){
  if (self == 0)
    return;
  int i;
  extern char *ns;
  CodeIndex *rs = (CodeIndex *) self;
  short int *si = (signed short *) val;
  
  rs->countrycodes = (BST) malloc(sizeof (BstNode) * lim);
  for (i = 0; i < lim; i++){
    rs->countrycodes[i]._lcp = rs->countrycodes[i]._rcp = (short)-1;
    rs->countrycodes[i]._keyid = ns;
  }
  rs->root = (short int) si;
  rs->n = -1;
  
}
/*********************************************************************************
 * Insert a index entry corresponding with a specified data record. */
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
    i = rs->root;
    while (i != -1){
      parent = i;
      if ((cond = strncmp(r->countrycode, rs->countrycodes[parent]._keyid, sizeof (r->countrycode))) == 0){
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
/*********************************************************************************
 *Give back memory space when done with BST. */
static void 
bstindex_destroy(void *self){
  if (self == 0)
    return;
  int i;
  CodeIndex *rs = (CodeIndex *) self;
  
  for (i = 0; i < rs->n; i++){
    free(rs->countrycodes[i]._keyid);
    free(rs->countrycodes);
  }
  rs->n = -1;
}
/*********************************************************************************
 *Function delegates the proper transaction fucntion that we want to preform on
 * BST index. */
static struct query
bstindex_transx(void *self, void *mode, void *target){
  struct query thisquery = {-1,0};
  if (self == 0)
    return thisquery;
  CodeIndex *rs = (CodeIndex *) self;
  ActionCode AC = *((int *) mode);
  if (rs->ops.t_helper[AC])
    thisquery = (rs->ops.t_helper[AC])(rs,target);
  return thisquery;
}

/*********************************************************************************
 *Binary search tree search. */
static struct query
bst_querycode(void *self, void *target){
  struct query results = {-1,0};
  signed int cond = 0;
  
  if (self == 0)
    return results;
  CodeIndex *rs = (CodeIndex *) self;
  int i = rs->root;

  do{
    results.ncomparisons++;
    cond = strncmp((char *) target, rs->countrycodes[i]._keyid, strlen(target));
    if (cond == 0)
      break;
    if (cond < 0)
      i = rs->countrycodes[i]._lcp;
    else 
      i = rs->countrycodes[i]._rcp;
  }while((i != -1));
  
  if (i == -1){
    /*failed search; the walk down the tree hit an external leaf.*/
    printf("ERROR - COUNTRY WITH CODE: %s.\n",(char *) target);
    return results;
  }
  else{
    results.success = rs->countrycodes[i]._drp;
    return results;
  }
}
/*********************************************************************************
 * Binary tree inorder traversal. */
static struct query
bst_inorder(void *self, void *target){
  struct query results = {1,0};
  if (self == 0)
    return results;
  CodeIndex *rs = (CodeIndex *) self;
  signed short int root = *((int *) target);
  
  if (root == -1)
    return results;
  bst_inorder(rs, &rs->countrycodes[root]._lcp);
  printf("%s.\n", rs->countrycodes[root]._keyid);
  bst_inorder(rs, &rs->countrycodes[root]._rcp);
    
  return results;
}
/*********************************************************************************
 * Methods assosciated with a index implemented by a BST on the backend. */
Storage bst_ops = {&bstindex_backup,
		   &bstindex_load,
		   &bstindex_init,
		   &bstindex_insert,
		   &bstindex_destroy,
		   &bstindex_transx,
		   {NULL,
		    NULL,
		    &bst_querycode,
		    NULL,
		    &bst_inorder,
		    NULL}
};
/*********************************************************************************
 * Helper function to bstindex_backup. Specific to the type of second argument. */
static void 
write_indice(FILE *f, BstNode n){
  fwrite(&n._lcp, sizeof (n._lcp), 1, f);
  fwrite(&n._rcp, sizeof (n._lcp), 1, f);
  fwrite(n._keyid, strlen(n._keyid)+1, 1, f); /* +1 to write out nul terminator. */
  fwrite(&n._drp, sizeof (n._drp), 1, f);
  return;
}		     
/*********************************************************************************
 * Helper function to bstindex_load.  Reads type BstNode from a binary stream. */
static BstNode
read_indice(FILE *f){
  int i;  
  BstNode n;
  char ks[] = {"\0\0\0"};
  fread(&n._lcp, sizeof (n._lcp), 1, f);
  fread(&n._rcp, sizeof (n._rcp), 1, f);
  for(i = 0; i < sizeof (ks) -1; i++){
    ks[i] = fgetc(f);
  }
  /*discard terminating null char associated w/ the 3char string.*/
  fgetc(f);
  n._keyid = strdup(ks);
  fread(&n._drp, sizeof (n._drp), 1, f);
  return n;
}
/*********************************************************************************
 * Helper function to bstindex_insert.  Specific to this projects internal array
 * representation of a binary tree.  NOTE: USING EXPLICIT CHILD POINTERS HERE.  */
static void
new_indice(CodeIndex *i, Record *r, unsigned int n){
  if (i != 0 && r != 0){
    i->countrycodes[n]._keyid = strndup(r->countrycode, sizeof (r->countrycode));
    i->countrycodes[n]._drp = r->id;
    i->countrycodes[n]._lcp = i->countrycodes[n]._rcp = -1;
    i->n++;
  }
}
/*********************************************************************************/
/*********************************************************************************/
