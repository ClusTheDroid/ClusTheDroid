#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "priq.h"
#include <time.h>
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

/* Defines similarity between two clusters, whether being a parent node or single instance. 
   The higher return value, the closer together the clusters are. */
double (*_similarity_function)(Node *node1, Node *node2);

Node **
_create_node_array(int columns, int rows)
{
	Node *values = calloc(columns*rows, sizeof(Node));
	Node **matrix = malloc(rows * sizeof(Node));
	int i;
	for(i=0; i<rows; i++) matrix[i] = values + i*columns;
	
	return matrix;
}

Node **
_construct_distance_matrix(	Node clusters[], int size, double (*sim)(Node *node_1, Node *node_2),
				pri_queue *priority_queues, int *active,
				double *sorted_pairwise_array, int *sorted_distances_length)
{
	Node **distance_matrix;
	distance_matrix = _create_node_array(size+1, size+1);

	pri_queue priqs;	
	priqs = priq_new(size);
	
	int n=1;
	for(; n<=size; n++)
	{
		active[n] = 1;
		priority_queues[n] = priq_new(size-1);
		clusters[n].n = n;
	
		int i=1;		
		for(; i<=size; i++)
		{
			distance_matrix[n][i].priority = _similarity_function(&clusters[n], &clusters[i]);
			distance_matrix[n][i].original_priority = distance_matrix[n][i].priority;
			distance_matrix[n][i].index = i;
			distance_matrix[n][i].n = n;

			if(i != n)
			{
				int *l = malloc(sizeof(int));
				*l = i;
				priq_push(priority_queues[n], l, distance_matrix[n][i].priority);
				if(i>n) priq_push(priqs, l, distance_matrix[n][i].priority);
			}
		}
	}
	
	/* Create a sorted array of the pairwise distances. This will be used for the calculation of C index */
	double tmp = 0.0;

	/* We do it this way, because we would like to get a segfault if we insert wrongly into array. Note - for debugging purposes */
	int i = 0;
	while(priq_top(priqs, &tmp))
	{
		priq_pop(priqs, &tmp);
		memcpy(&sorted_pairwise_array[i++], &tmp, sizeof(double));
	}
	
	return distance_matrix;
}

Node *
_copy_node(int k1, Node clusters[])
{
	Node *tmp = calloc(1, sizeof(Node));
	memcpy(tmp, &clusters[k1], sizeof(Node));
	return tmp;
}

Node *
merge_clusters(int k1, int k2, Node clusters[], FeatureSet* (*_calculate_merged_features)(int, int, Node clusters[]))
{	
		Node *tmp_k1 = calloc(1, sizeof(Node));
		
		tmp_k1->index = clusters[k1].index;
		tmp_k1->is_parent = 1;
		tmp_k1->left = _copy_node(k1, clusters);
		tmp_k1->right = _copy_node(k2, clusters);
		tmp_k1->size = clusters[k1].size + clusters[k2].size;	
		tmp_k1->feature_set = _calculate_merged_features(k1, k2, clusters);

		clusters[k1] = *tmp_k1;
			
		return tmp_k1;
}

void 
_print_distance_matrix(Node **distance_mtrx, int size)
{
	int i=1;
	for(; i <= size; i++)
	{
		int n=1;
		for(; n <= size; n++)
		{
			printf("%f\t", distance_mtrx[i][n].priority);
		}
		printf("\n");
	}
}

void
_print_cluster(Node *tree)
{
	if(tree->is_parent)
	{
		_print_cluster(tree->left);
		_print_cluster(tree->right);
	}
	else
	{
		Instance *inst = (Instance *) tree->data;
		printf("%s, ", inst->family_name);
	}
}

void
_print_tree(char *prefix, int is_tail, Node *tree)
{
	if(tree->is_parent) printf("%s %s %.3f\n", prefix, (is_tail ? "└──" : "├──"), tree->value);
	else
	{
		Instance *inst = (Instance *)tree->data;
		printf("%s %s %s %.25s\n", prefix, (is_tail ?  "└──" : "├──"), inst->family_name, inst->path);
		//printf("%s %s %-15s | %.35s %.2f \n", prefix, (is_tail ? "└──" : "├──"), inst->family_name, inst->path, 
							//		inst->feature_set->feature_vector[0]);
	}		

	if(tree->is_parent)
	{
		char *left = malloc(sizeof(char)*strlen(prefix)+10);
		strcpy(left, prefix);
		strcat(left, is_tail ? "     " : " │   ");	
		_print_tree(left, 0, tree->left);
		
		char *right= malloc(sizeof(char)*strlen(prefix)+10);
		strcpy(right, prefix);
		strcat(right, is_tail ? "     " : " │   ");
		_print_tree(right, 1, tree->right);
		
		free(left);
		free(right); 
	}
}

double
SingleLink(int node1, int merge1, int merge2, Node **distance_matrix, Node *clusters)
{
	return max(distance_matrix[node1][merge1].priority, distance_matrix[node1][merge2].priority);
}

double
CompleteLink(int node1, int merge1, int merge2, Node **distance_matrix, Node *clusters)
{
	return min(distance_matrix[node1][merge1].priority, distance_matrix[node1][merge2].priority);
}

double
CentroidLink(int node1, int node2, int garbage, Node **distance_matrix, Node *clusters)
{
	return _similarity_function(&clusters[node1], &clusters[node2]);
}

void
get_maximum_number_of_instances(Node *tree, hashtable_t *stats)
{
	if(tree->is_parent)
	{
		get_maximum_number_of_instances(tree->left, stats);
		get_maximum_number_of_instances(tree->right, stats);
	}
	else
	{
		int n = hashtable_get_value(stats, ((Instance *) tree->data)->family_name);
		put_hash(stats, ((Instance *)tree->data)->family_name, n+1);
	}
}

double
calculate_recall(Node clusters[], hashtable_t *statistics, int size, int *active)
{
	int max_number_of_nodes = 0;
	
	hashtable_t *stats = create_dictionary(10);
	hashtable_t *maximum_stats = create_dictionary(10);
	
	key_w *keys = statistics->keys;
	
	key_w *tmp = keys;
	for(; tmp != NULL; tmp = tmp->next) 
	{
		put_hash(maximum_stats, tmp->key, 0);
	}

	int i = 1;
	for(; i<=size; i++)
	{
		if(active[i])
		{	
			key_w *t_k = keys;
			for(; t_k != NULL; t_k = t_k->next) put_hash(stats, t_k->key, 0);
			
			/* Insert new stats into the zero'ed hashtable */
			get_maximum_number_of_instances(&clusters[i], stats);

			for(t_k = keys; t_k != NULL; t_k = t_k->next)
			{
				if(hashtable_get_value(stats, t_k->key) > hashtable_get_value(maximum_stats, t_k->key))
					put_hash(maximum_stats, t_k->key, hashtable_get_value(stats, t_k->key));
			}
		}	
	}

	for(; keys != NULL; keys = keys->next) max_number_of_nodes += hashtable_get_value(maximum_stats, keys->key);
	
	return (double)max_number_of_nodes / (double)size;
}

double
calculate_precision(Node clusters[], hashtable_t *statistics, int size, int *active)
{
	double precision;
	int max_number_of_nodes = 0;
	
	hashtable_t *stats = create_dictionary(10);
	key_w *keys = statistics->keys;
	int i = 1;
	for(; i<=size; i++)
	{
		if(active[i])
		{
			key_w *t_k = keys;
	
			for(; t_k != NULL; t_k = t_k->next) put_hash(stats, t_k->key, 0);
			
			get_maximum_number_of_instances(&clusters[i], stats);			
			
			int max_value = 0;
			for(t_k = keys; t_k != NULL; t_k = t_k->next)
			{
				if(hashtable_get_value(stats, t_k->key) > max_value)
				{
					max_value = hashtable_get_value(stats, t_k->key);
					
				}
			}
			max_number_of_nodes += max_value;
		}
	}
	precision =  (double)max_number_of_nodes / (double)size;
	return precision;
}


void
GetArrayOfNodes(Node *cluster, Node clusters[], int *value)
{
	if(cluster->is_parent)
	{
		GetArrayOfNodes(cluster->left, clusters, value);
		GetArrayOfNodes(cluster->right, clusters, value);
		return;
	}
	else
	{
		clusters[*value] = *cluster;
		*value = *value+1;
		return;
	}
}

double
CalculateCIndex(Node clusters[], int active[], int size, Node **distance_matrix, double *sorted_pairwise_array, double ramudjan)
{
	/* calculate the accumulated sum of all distances between pairs, in given clusters */
	int acc_size = 0;
	double S = 0.0;
	int i;
	for(i=1; i<=size; i++) 
	{
		if(active[i])
		{
			Node *cluster_array = calloc(clusters[i].size, sizeof(Node));
			int incr = 0;
			GetArrayOfNodes(&clusters[i], cluster_array, &incr);
			
			int n, m;
			for(n=0; n<incr; n++)
			{
				for(m=n+1; m<incr; m++)
				{
					acc_size++;
					S += 1.0-distance_matrix[cluster_array[n].n][cluster_array[m].n].original_priority;
				}
			}
		}
	}

	
	double s_max, s_min;
	s_max = s_min = 0.0;	
	
	for(i=0; i<acc_size; i++)
	{
		 s_min += 1.0 - sorted_pairwise_array[i];	
	}

	for(i=ramudjan-1; i > (ramudjan-1)-acc_size; i--)
	{
		 s_max += 1.0 - sorted_pairwise_array[i];
	}	
	double numerator = S - s_min;
	double denominator = s_max - s_min;
	if(denominator <= 0.0) return 0.0;
	
	double c_index =  numerator / denominator;
	
//	printf("S: %f \t S_min: %f \t S_max: %f \t nume: %f denom: %f  c_index %f\n", S, s_min, s_max, numerator, denominator,  c_index);
	return c_index;
}

void
EfficientHierarchicalClustering(Node clusters[], int size, 
				double (*sim)(Node *node_1, Node *node_2),
				double (*linkage)(int, int, int, Node**, Node *),
				hashtable_t *statistics,
				FeatureSet *(*_calculate_merged_features)(int k1, int k2, Node clusters[]), int unknown, int merge_stop)
{
	int unknown_malware = unknown;

	/* getting ready for the sorted array */
	double ramudjan =((double)(size-1) * ((double)(size-1) + 1.0))/2.0;
	
	pri_queue *priqs = calloc(size+1, sizeof(pri_queue));
	int *active = calloc(size+1, sizeof(int));
	double *sorted_pairwise_array = calloc(ramudjan, sizeof(double));
	int *sorted_array_length = malloc(sizeof(int));


	_similarity_function = sim;
	clock_t start, end;
	start = clock();
	Node **distance_matrix = _construct_distance_matrix(clusters, size, sim, priqs, active, sorted_pairwise_array, sorted_array_length);
	end = clock();
	printf("Computing distance matrix: %f\n", (end-start)/(double)CLOCKS_PER_SEC);
	/* line 9 and forth */
	// Outerloop to do all the merges	
	int k = 1;
	double old_sim = 1.1;
	start = clock(); 
	for(; k<size; k++)
	{
		// line 10 - find the minimum distance now
		double max_prior = 0.0;
		int *k1 = calloc(1, sizeof(int));
		int *k2 = calloc(1, sizeof(int));
	
		int i=1;
		for(; i<=size; i++)
		{
			if(active[i])
			{
				double tmp_prior = 0.0;
				int *tmp = NULL;
				
				/* Skip the elements in the priority queue that are old and "should have" been deleted */
				while(priq_top(priqs[i], &tmp_prior))
				{
					tmp = priq_top(priqs[i], &tmp_prior);
					
					if(active[*tmp]) break;
					
					priq_pop(priqs[i], &tmp_prior); 
					tmp_prior = 0.0;
				}

				if(tmp_prior >= max_prior)
				{
					/* Check whether the value in the priority queue is an old value */
					if(distance_matrix[i][*tmp].priority != tmp_prior)
					{
						priq_pop(priqs[i--], &tmp_prior);
						continue;
					}
					else
					{
						/* Update max value, and set the index of the cluster with highest score */
						*k1 = i;
						max_prior = tmp_prior;
					}
				}
			}
		}
		/* Get the second cluster for the merge */
		k2 = priq_pop(priqs[*k1], &max_prior);

		/* k1 and k2 are the indexes in clusters[] of the cluster's we want to merge */
		Node *new_k1 = merge_clusters(*k1, *k2, clusters, _calculate_merged_features);
		new_k1->value = (double) k;
		clusters[*k1] = *new_k1;
		active[*k2] = 0;
	
		int lm = 1;
		int activeClusters = 0;
		for(; lm<=size; lm++) if(active[lm]) activeClusters++;
		
		if(unknown_malware)
		{
			double c_index = CalculateCIndex(clusters, active, size, distance_matrix, sorted_pairwise_array, ramudjan);
			printf("%d\t%f\t%f\t\n", k, max_prior, c_index);
		}
		else
		{
			double precision = calculate_precision(clusters, statistics, size, active);
			double recall = calculate_recall(clusters, statistics, size, active);
			double c_index = CalculateCIndex(clusters, active, size, distance_matrix, sorted_pairwise_array, ramudjan);
			
			if(old_sim > max_prior){
				printf("%d\t%f\t%f\t%f\t%f\t%d\n", k, max_prior, precision, recall, c_index, activeClusters);
				old_sim = max_prior;
			}
			if(k == merge_stop)
			{
				int i = 1;
				int n = 1;
				//printf("{\n \"Clusters\": [");
				for(; i<=size; i++)
				{
					if(active[i]) 
					{
						printf("cluster:%d [", n);
						_print_cluster(&clusters[i]);
						printf("]\n");
						n++;
					}
				}
				//printf(" ]\n}\n");
			}			
		}

		priq_clear(priqs[*k1]);
		priqs[*k1] = priq_new(0);
		/* update priority queues */
		for(i=1; i <= size; i++)
		{
			if(active[i] && i != *k1)
			{
				/* Calculate new similarities when the clusters get merged */	
				double new_sim = linkage(i, *k1, *k2, distance_matrix, clusters);	
				distance_matrix[i][*k1].priority = new_sim;
				distance_matrix[*k1][i].priority = new_sim;
				
				/* Update priority queues */
				/* Line 19 */
				int *index_1 = malloc(sizeof(int));	
				*index_1 = *k1;
				priq_push(priqs[i], index_1, new_sim);

				/* Line 21 */
				int *index_2 = malloc(sizeof(int));
				*index_2 = i;
				priq_push(priqs[*k1], index_2, new_sim);
			}
		}
	}
/*	int i = 1;	
	for(; i <= size; i++)
	{
		if(active[i])	
		{
			_print_tree("", 1, &clusters[i]);
		}
	}*/
	end = clock();
	printf("Clustering: %f \n", (end-start)/(double)CLOCKS_PER_SEC);
}
