struct key_s{
        char *key;
        struct key_s *next;
};

typedef struct key_s key_w;

struct entry_s{
        char *key;
        int value;
        struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s{
        int size;
        struct entry_s **table;
        struct key_s *keys;
};

typedef struct hashtable_s hashtable_t;


hashtable_t * create_dictionary(int size);

int hash(char *key, hashtable_t *hashtable);

void put_hash(hashtable_t *hashtable, char *key, int value);


int hashtable_get_value(hashtable_t *hashtable, char *key);
