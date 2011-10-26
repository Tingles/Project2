
enum {linemax = 400, maxcountries = 300};
enum Offsets {null_termin = 1, alignment_offset = 3};
enum Actions {Query_name, Query_keyid, Query_code, List_keyid, List_code, List_name};

typedef enum Offsets Offset;
typedef enum Actions ActionCode;
typedef struct Storage Storage;
typedef struct BstNode *BST;
typedef struct BstNode BstNode;
typedef struct CodeIndex CodeIndex;
typedef struct CountryStorage CountryStorage;
typedef struct Record *Recs;
typedef struct Record Record;
typedef struct meta Meta;

struct meta{
  signed short nrecords;
  signed short root;
  unsigned short maxid;
};

struct Storage{
  void (*backup)(void *self, FILE *f);  /*functions that storage may need.*/
  void (*load)(void *self, FILE *f);
  void (*init)(void *self, void *val, int lim);
  void (*insert)(void *self, void *data);
  void (*destroy)(void *self);
  int  (*transact)(void *self, void *query_mode, void *target);
  int  (*t_helper[6])(void *self, void *target);
};

struct BstNode{
  signed short int _lcp; /* left child pointer. */
  signed short int _rcp; /* right child pointer. */
  char *_keyid; /* key used on queries. */
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
  char countrycode[3];      //3 char code.
  char name[17];            //left justified, space-filled.
  char continent[11];       // *                         *
  char region[10];          // *                         *
  unsigned int surfacearea;          //32 bit data.
  signed short int yearindep;      //16 bit data.
  unsigned long int population;      //64 bit data.
  float lifexp;            //32 bit data.
  unsigned int gnp;                  //32 bit data.
};

struct CountryStorage{
  Storage ops;
  Recs *countries;
  unsigned short maxid;
  signed short nrecords;
};


extern Record *record_new(char *);
extern void dumpheader(FILE *, short */*nrecs*/, short */*root*/, short */*maxid*/); 
extern Meta readheader(FILE *);
extern void initialize_env(CountryStorage *, CodeIndex *, Meta *);
extern char *parsetransrec(char*);
extern char *strip(char*);
extern ActionCode action2enum(char *);
