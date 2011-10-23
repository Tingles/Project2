/* Author : Ian Kane
 * Setup is the first of four components which comprise 
 * an application to provide quick access to data about the countries of the world.
 * The user can access data about an individual country or about all countries.
 */

#define ADS ActualDataStorage /*macro subsitution to shorten lengthy expresions.*/

#include <stdio.h>
#include <stdlib.h>
#include "./setup.h"

static  CodeIndex     CountryIndex;
static  CountryStorage ActualDataStorage;


int main (int argc, char * argv[])
{
  extern Storage array_ops;
  extern Storage bst_ops;
  char buff[linemax];
  FILE  *input, *backup;
  
  if ((input = fopen(argv[1],"r")) == 0){
    printf("Problem opening data file.\n");
    exit(EXIT_FAILURE);
  }
  if ((backup = fopen("./backup.bin", "wb")) == 0){
    printf("Problem opening backupfile.\n");
    exit(EXIT_FAILURE);
  }
  
  /*
   * What kind of storage and index do we have?
   * In part one, we use an array for storage.
   * And a BST(array) for keeping tabs on the data stored.
   */
  ActualDataStorage.ops = array_ops;
  CountryIndex.ops = bst_ops;

  (ActualDataStorage.ops.init) (&ActualDataStorage, (Record *) 0, maxcountries);
  (CountryIndex.ops.init) (&CountryIndex, 0, maxcountries);

  while(fgets(buff,linemax,input) != 0){
    Record *c = record_new(buff);
    (ActualDataStorage.ops.insert) (&ActualDataStorage, c);
    (CountryIndex.ops.insert) (&CountryIndex, c);
  }
  fclose(input);
  dumpheader(backup, &ADS.nrecords, &CountryIndex.root, &ADS.maxid);
  (ActualDataStorage.ops.backup) (&ActualDataStorage, backup);
  (CountryIndex.ops.backup) (&CountryIndex, backup);
  (ActualDataStorage.ops.destroy) (&ActualDataStorage);
  (CountryIndex.ops.destroy) (&CountryIndex);
  fclose(backup);
  exit(EXIT_SUCCESS);
}

void
dumpheader(FILE *f, short *nrecs, short *root, short *maxid){
  fwrite(nrecs, sizeof(short), 1, f);
  fwrite(root, sizeof(short), 1, f);
  fwrite(maxid, sizeof(short), 1, f);
}
