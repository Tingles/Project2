
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "./setup.h"

/*globals*/
static CodeIndex      CountryIndex;
static CountryStorage ActualDataStorage;

  
int main(int argc, char * argv[]){
  
  extern Storage array_ops;
  extern Storage bst_ops;
  FILE *input;
  char buff[linemax];
  Meta header = {0,0,0};
  
  if((input = fopen("../backup.bin","rb+")) == NULL){
    printf("Problem opening data file.\n");
    exit(EXIT_FAILURE);
  }
  /*read header record for file meta data.*/
  header = readheader(input);
  
  /*specify backend storage/index methods.*/
  CountryIndex.ops = bst_ops;
  ActualDataStorage.ops = array_ops;

  /*initalize data structures*/
  initialize_env(&ActualDataStorage, &CountryIndex, &header);
  /*
   *read datastructures from binary backup.  Index must be read first because thats
   * how it was written out.
   */
  (CountryIndex.ops.load) (&CountryIndex, input);
  (ActualDataStorage.ops.load) (&ActualDataStorage, input);
  
  /*Done loding data from backup, open stream for transaction.*/
  fclose(input);
  if ((input = fopen(argv[1], "r")) == 0){
    printf("Problem opening data file.\n%m\n");
    exit(EXIT_FAILURE);
  }
  while(fgets(buff,linemax,input) != 0){
    char *target, *action;
    ActionCode AC;
    struct query thisquery;
    
    printf("%s", buff);
    
    target = strip(parsetransrec(buff));
    action = buff;
    AC = action2enum(action);
    
    switch(AC){
    case Query_name:{
      /*sequential search through ActualDataStorage.*/
      thisquery = (ActualDataStorage.ops.transact) (&ActualDataStorage, &AC, target);
      printf(">> %d probes.\n", thisquery.ncomparisons);
    }break;
    case Query_keyid:{
      /*
       *Direct address in ActualDataStorage, record is either there or it's not.
       */
      thisquery = (ActualDataStorage.ops.transact) (&ActualDataStorage, &AC, (void *) atoi(target));
      printf(">> %d probes.\n", thisquery.ncomparisons);

    }break;
    case Query_code:{
      /*
       *BST walk, through CountryIndex., at worst case the # of comparisons is 
       * the height of the tree.
       */
      int drp;
      struct query extraquery = {-1,0};
      thisquery = (CountryIndex.ops.transact) (&CountryIndex, &AC, target);
      if ((drp = thisquery.success) > 0){
	ActionCode tmp = Query_keyid;
	extraquery = (ActualDataStorage.ops.transact) (&ActualDataStorage,&tmp,(void *) drp);
	printf(">> %d + %d probes.\n", thisquery.ncomparisons, extraquery.ncomparisons);
      }
      else{
	printf(">> %d + %d probes.\n",thisquery.ncomparisons, extraquery.ncomparisons);
      }
    }break;
    case List_name:{
      /*
       *What is this?
       */
	printf("List by name is not implemented.\n");
    }break;
    case List_keyid:{
      /*
       *List all countries by Id. This could be a sequential traversal over 
       * ActualDataStorage.
       */
      (ActualDataStorage.ops.transact) (&ActualDataStorage, &AC, target);
    }break;
    case List_code:{
      /*
       *List all countries by Code. Implementation is an *inorder* traversal of
       *  our BST.
       */
      (CountryIndex.ops.transact) (&CountryIndex, &AC, &CountryIndex.root);
    }break;
    default:break;
    }
  }
  exit(EXIT_SUCCESS);
}
