#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jsmn.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include "hash.h"

/* Used to define the amount of bytes read at maximum from a read systemcall */
#define READ_SIZE 4095
#define use_procnames 0
#define use_filenames 0
#define use_execnames 0
#define use_file_alpha 0
#define use_exec_alpha 0

#define highlevel_behaviour 1
#define midlevel_behaviour 2
#define lowlevel_behaviour 3



#define _size 0
#define _personal 1
#define _phone 2
#define _sms 12
#define _account 15
#define _networkk 18
#define _execute 23
#define _call 28	
#define _FS 29
#define _SMS_send 30
#define _MakeCall 31
#define _location 32
#define _contacts 39
#define _proccount 41
#define _filecount 42
#define _execcount 43
#define _feature_vector_size 44

typedef struct Family Family;
typedef struct Instance Instance;
typedef struct Root Root;
typedef struct FeatureSet FeatureSet;

struct Root{
	Family *families;
};

struct Family{
	char *name;
	Family *next;
	Instance *instances;
};

struct Instance{
	char *path;
	char *family_name;
	FeatureSet *feature_set;	
	Instance *next;
};

struct FeatureSet{
	double *feature_vector;
	hashtable_t *procnames;
	hashtable_t *filenames;
	hashtable_t *execnames;
};

/*	Takes a token as input, and prints out the content of it, according to the Token offsets in the given text */
char * printToken(char *text, jsmntok_t *token);

void _access_personal_info_PHONE(char *feature, int offset, double *feature_vector, FeatureSet *features);
void _access_personal_info_SMS(char *feature, int offset, double *feature_vector, FeatureSet *features);
void _access_personal_info_ACCOUNT(char *feature, int offset, double *feature_vector, FeatureSet *features);


/*	Constructs the tokens corresponding to the given string, and returns a pointer to these tokens. */
jsmntok_t * getTokens(char *js);

/*	Extracts the features of the char pointer */
//FeatureSet * extractFeatures(char *content, hashtable_t *stats);

/*	Parses the input and returns a featureset */
Family * parse(char *directory);

/*	This is used to iterate through the json directory and find all the json files.
	It constructs a directory tree with the file names attached to it. */
int listFiles(char *pathName, Family *head);

/*	This reads the content of the specified filename and returns a pointer to the content. */
char * readFile(char *filename);

/*	Returns the number of elements in the tree */
int FamilySize(Family *root, double featureSize, double max_nr_of_features);

hashtable_t * ListFamilyNames(Family *root, double min_nr_of_features);

