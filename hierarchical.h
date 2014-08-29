#include "parser.h"

#define max(a,b) \
        ({typeof(a) _a = (a); \
          typeof(b) _b = (b); \
         _a > _b? _a : _b;})


#define min(a,b) \
        ({typeof(a) _a = (a); \
          typeof(b) _b = (b); \
         _a < _b? _a : _b;})


typedef struct Node {
        void *data;
        double priority, original_priority, value;
        int index, n, size, is_parent;
        struct Node *left, *right;
        FeatureSet *feature_set;
} Node;


double
CompleteLink(int node1, int merge1, int merge2, Node **distance_matrix, Node *clusters);

double
SingleLink(int node1, int merge1, int merge2, Node **distance_matrix, Node *clusters);
void
EfficientHierarchicalClustering(Node clusters[], int size,
                                double (*sim)(Node *node_1, Node *node_2),
                                double (*linkage)(int, int, int, Node**, Node *),
                                hashtable_t *statistics,
                                FeatureSet *(*_calculate_merged_features)(int k1, int k2, Node clusters[]), int unknown, int merge_stop);

