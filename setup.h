

enum {linemax = 400, maxcountries = 300};

typedef struct Storage Storage;
typedef struct BstNode *BST;
typedef struct BstNode BstNode;
typedef struct CodeIndex CodeIndex;
typedef struct CountryStorage CountryStorage;
typedef struct Record *Recs;
typedef struct Record Record;


struct Storage{
  void (*backup)(void *self, FILE *f);  /*functions that storage may need.*/
  void (*init)(void *self, void *val, int lim);
  void (*insert)(void *self, void *data);
  void (*destroy)(void *self);
};

struct BstNode{
  signed short int _lcp; /* left child pointer. */
  signed short int _rcp; /* right child pointer. */
  unsigned char *_keyid; /* key used on queries. */
  unsigned int _drp; /* index into ActualDataStorage. */
};

struct CodeIndex{
  Storage ops;
  BST countrycodes;
  signed short int root;
  signed short int n;
};


struct Record{ 
  unsigned short int id;             //16 bit id.
  unsigned char countrycode[3];      //3 char code.
  unsigned char name[17];            //left justified, space-filled.
  unsigned char continent[11];       // *                         *
  unsigned char region[10];          // *                         *
  unsigned int surfacearea;          //32 bit data.
  signed short int yearIndep;      //16 bit data.
  unsigned long int population;      //64 bit data.
  float lifexp;            //32 bit data.
  unsigned int gnp;                  //32 bit data.
};

struct CountryStorage{
  Storage ops;
  Recs *countries;
  unsigned short maxid;
  unsigned short nrecords;
};


extern Record *record_new(char *);
extern void handlerec(Record);
extern void dumpheader(FILE *, short */*nrecs*/, short */*root*/, short */*maxid*/); 
