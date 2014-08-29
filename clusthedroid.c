#include <stdio.h>
#include <stdlib.h>
#include "priq.h"
#include "hierarchical.h"
#include <ctype.h>
#include <time.h>

static int unknown_malware;
static int linkage;
static char *jsons;
static int merge_stop = -1;



int
compareStrings(char *text)
{
	int numerical = 0;
	int digit = 0;
	char c;
		
	while((c = *text))
	{
		if(isalpha(c)) digit++;
		else if(isdigit(c)) numerical++;
		text++;
	}
	if(digit > numerical) return 1;
	else{ return 0; }
}

Node *
GetClusters(Family *root, double min_nr_features, double max_nr_features, int *size)
{
	*size = FamilySize(root, min_nr_features, max_nr_features);
	Node *nodes = calloc(*size+1, sizeof(Node));

	int i=1;
	while(root != NULL)
	{
		if(root->instances != NULL)
		{
			Instance *tmp = root->instances;
			double tmp_size = 0.0;
			while(tmp != NULL)
			{
				if(tmp->feature_set->feature_vector[0] >= min_nr_features && tmp->feature_set->feature_vector[0] <= max_nr_features)
				{
					nodes[i].value = i+0.0;
					nodes[i].is_parent = 0;
					nodes[i].data = tmp;
					nodes[i].size = 1;
					i++;
					tmp_size++;
				}
				tmp = tmp->next;
			}
			if(tmp_size > 0.0)
			{
				printf("%-25s \t size: %.4f\n", root->name, tmp_size);
			}
		}
		root = root->next;
	}
	printf("done med clusters \n");	
	
	return nodes;
}

FeatureSet *
_calculate_merged_featureset(int k1, int k2, Node clusters[])
{
 /*               FeatureSet *fs1 = clusters[k1].is_parent ? clusters[k1].feature_set : ((Instance *) clusters[k1].data)->feature_set;
                FeatureSet *fs2 = clusters[k2].is_parent ? clusters[k2].feature_set : ((Instance *) clusters[k2].data)->feature_set;

                double s1 = (double)clusters[k1].size;
                double s2 = (double)clusters[k2].size;
*/
                FeatureSet *new_fw = calloc(1, sizeof(FeatureSet));
                /*
		new_fw->FS = ((fs2->FS * s2) + (fs1->FS * s1)) /  (s2+s1);
                new_fw->PERSONAL = ((fs2->PERSONAL * s2) + (fs1->PERSONAL * s1)) / (s2+s1);
                new_fw->NETWORK = ((fs2->NETWORK * s2) + (fs1->NETWORK * s1)) / (s2+s1);
                new_fw->EXECUTE = ((fs2->EXECUTE * s2) + ((fs1->EXECUTE) * s1)) / (s2+s1);
		*/
                return new_fw;
}

int
match_k_string(char *one, char *second, int k_gram)
{
	if(strcmp(one, second)==0) return 1;
	if(strlen(one) < k_gram || strlen(second) < k_gram) return 0;

	int strleng1 = strlen(one);
	int i;
	
	for(i=0; i+k_gram <= strleng1; i++)
	{
		char *subsub = calloc(k_gram, sizeof(char));
		
		strncpy(subsub, one+i, k_gram);
		if(strstr(second, subsub))
		{
			free(subsub);
			return 1;
		}
		free(subsub);
	}
	return 0;
}

double
SimilarityScore(Node *node1, Node *node2)
{
//	printf("[+] begin similarity [+]\n");
	FeatureSet *set1 = node1->is_parent ? node1->feature_set : ((Instance *) node1->data)->feature_set;
        FeatureSet *set2 = node2->is_parent ? node2->feature_set : ((Instance *) node2->data)->feature_set;
	double score_2 = 0.0;
	double set_intersection = 0.0;
	double set_union = 0.0;

	int m;
	for(m=1; m<_feature_vector_size-3; m++) 
	{ 
		set_intersection += min(set1->feature_vector[m], set2->feature_vector[m]); 
	}	

	for(m=1; m<_feature_vector_size-3;m++)
	{
		set_union += max(set1->feature_vector[m], set2->feature_vector[m]);
	}

	if(use_procnames)
	{
		double proc_union = 0.0;
		double proc_intersection = 0.0;
		
		proc_union += set1->feature_vector[_proccount];
		proc_union += set2->feature_vector[_proccount];
		key_w *keys = set1->procnames->keys;
		for(; keys != NULL; keys = keys->next)
		{
			key_w *keys2 = set2->procnames->keys;
			for(; keys2 != NULL; keys2 = keys2->next)
			{
				if(strcmp(keys2->key, keys->key) == 0) proc_intersection++;
			}
		}
		proc_union -= proc_intersection;
		proc_union /= 5;
		proc_intersection /= 5;

		set_intersection += proc_intersection;
		set_union += proc_union;
	}

	
	if(use_file_alpha)
	{
		int digits, numericals;
		digits = numericals = 0;
		
		key_w *keys = set1->filenames->keys;
		for(; keys != NULL; keys = keys->next)
		{
			if(compareStrings(keys->key)) digits++;
			else{ numericals++;}
		}

		int digits2, numericals2;
		digits2 = numericals2 = 0;

		key_w *keys2 = set2->filenames->keys;
		for(; keys2 != NULL; keys2 = keys2->next)
		{
			if(compareStrings(keys2->key)) digits2++;
			else{ numericals2++;}
		}
			
		double digit_intersect = min(digits, digits2);
		double digit_union = max(digits, digits2);

		double numerical_intersect = min(numericals, numericals2);
		double numerical_union = max(numericals, numericals2);

		digit_intersect /= 5;
		digit_union /= 5;
		numerical_intersect /= 5;
		numerical_union /= 5;

		set_intersection += (digit_intersect+numerical_intersect);
		set_union += (digit_union + numerical_union);	
	}	

	if(use_filenames)
	{
		double file_union = 0;
		double file_intersection = 0;
		file_union += set1->feature_vector[_filecount];
		file_union += set2->feature_vector[_filecount];
		
		key_w * keys = set1->filenames->keys;
		for(; keys != NULL; keys = keys->next)
		{
			key_w *keys2 = set2->filenames->keys;
			for(;keys2 != NULL; keys2 = keys2->next)
			{
				if(strcmp(keys->key, keys2->key) == 0) file_intersection++;
			}
		}
		file_union -= file_intersection;
		file_union /= 4;
		file_intersection /= 4;
		
		set_intersection += file_intersection;
		set_union += file_union;
	}
	
	if(use_execnames)
	{
		double exec_union = 0;
		double exec_intersection = 0;
		
		exec_union += set1->feature_vector[_execcount];
		exec_union += set2->feature_vector[_execcount];
	
		key_w *keys = set1->execnames->keys;
		for(; keys != NULL; keys = keys->next)
		{
			key_w *keys2 = set2->execnames->keys;
			for(; keys2 != NULL; keys2 = keys2->next)
			{
				if(strcmp(keys->key, keys2->key)==0) exec_intersection++;
			}
		}
		exec_union -= exec_intersection;
		exec_union /= 4;
		exec_intersection /= 4;
	
		set_intersection += exec_intersection;
		set_union += exec_union;
	}
		
	if(use_exec_alpha)
	{
		int digits, numericals;
		digits = numericals = 0;
		
		key_w *keys = set1->execnames->keys;
		for(; keys != NULL; keys = keys->next)
		{
			if(compareStrings(keys->key)) digits++;
			else{ numericals++;}
		}

		int digits2, numericals2;
		digits2 = numericals2 = 0;

		key_w *keys2 = set2->execnames->keys;
		for(; keys2 != NULL; keys2 = keys2->next)
		{
			if(compareStrings(keys2->key)) digits2++;
			else{ numericals2++;}
		}
			
		double digit_intersect = min(digits, digits2);
		double digit_union = max(digits, digits2);

		double numerical_intersect = min(numericals, numericals2);
		double numerical_union = max(numericals, numericals2);

		digit_intersect /= 5;
		digit_union /= 5;
		numerical_intersect /= 5;
		numerical_union /= 5;

		set_intersection += (digit_intersect+numerical_intersect);
		set_union += (digit_union + numerical_union);	
	}	
	
	score_2 = set_intersection/set_union;
	return score_2;
}



void 
parse_arguments(int argc, char **argv)
{
	if(argc < 2)
	{
		printf("Uses samples from ./jsons/genome_titill_jsons/\n");
		printf("Clusters with Single-Linkage and calculates precision, recall and c-index\n");
		// Do the standard options. 
	}
	else
	{
		int i = 1;
		for(; i<argc; i++)
		{
			if(strcmp("-l", argv[i]) == 0)
			{
				if(strcmp("Single", argv[i+1]) == 0)
				{
					linkage = 1;
				}	
				else if(strcmp("Complete", argv[i+1]) == 0)
				{
					linkage = 2;
				}
				else
				{
					printf("Unknown linkage criteria\n");
					exit(0);
				}
				i++;
			}
			else if(strcmp("-d", argv[i]) == 0)
			{
				printf("Directory: %s\n", argv[i+1]);
				jsons = argv[i+1];
				i++;
			}
			else if(strcmp("-u", argv[i]) == 0)
			{
				printf("Unknown malware\n");
				unknown_malware = 1;
			}
			else if(strcmp("-k", argv[i]) == 0)
			{
				merge_stop = atoi(argv[i+1]);
				printf("merge_stop: %d\n", merge_stop);	
			}
		}
	}	
}

int
main(int argc, char **argv)
{
	clock_t start, end;
	double min_nr_of_features = 5.0;

	parse_arguments(argc, argv);

	double (*linkageFunction) (int, int, int, Node **, Node *);
	if(linkage == 2) linkageFunction = CompleteLink;
	else{ linkageFunction = SingleLink; }	
	printf("linkage: %d unkown: %d directory: %s \n", linkage, unknown_malware, jsons);
	
	// Parse	
	printf("Parsing\n");
	start = clock();
	Family *root = parse(jsons);
	end = clock();
	printf("Done parsing. \nTime to parse and generate signatures: %f\n", (end-start)/(double)CLOCKS_PER_SEC);
	
	//Extract samples that can be clustered
	int size;
	Node *nodes = GetClusters(root, min_nr_of_features, 9999900.0, &size);	
	printf("Size of samples to get clustered %d\n", size);	
	hashtable_t *hashtable = ListFamilyNames(root, min_nr_of_features);
	end = clock();
	
	// Perform clustering
	printf("Starting HAC\n");

	start = clock();
	EfficientHierarchicalClustering(nodes, size, SimilarityScore, linkageFunction, hashtable, _calculate_merged_featureset, unknown_malware, merge_stop);
	end = clock();
	return 1;	
}
