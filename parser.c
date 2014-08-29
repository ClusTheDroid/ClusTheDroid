#include "parser.h"

/*
* Return the tokens of a string in javascript format. 
*/
jsmntok_t * 
getTokens(char *js)
{
	jsmn_parser parser;
	jsmn_init(&parser);

	unsigned int size = 1048;
	jsmntok_t *tokens = calloc(size, sizeof(jsmntok_t));

	int ret = jsmn_parse(&parser, js, strlen(js), tokens, size);
	
	while(ret == JSMN_ERROR_NOMEM)
	{
		size = size*2;
		jsmntok_t *token = realloc(tokens, size * sizeof(jsmntok_t));
		if(!token)
		{
			printf("failed to allocate tokens \n");
		}
		jsmn_init(&parser);
		tokens = token;
		ret = jsmn_parse(&parser, js, strlen(js), tokens, size);	
	}
	if(ret == JSMN_ERROR_INVAL){
		printf("jsmn_parse: invalid JSON \n");
		exit(0);
	}

	if(ret == JSMN_ERROR_PART)
	{
		printf("jsmn_parse: truncated JSON string \n");
		exit(0);
	}
	return tokens;
}


/*
* Return the value of a token in a calloc'ed string.
*/
char *
printToken(char *text, jsmntok_t *token)
{
	int size = token->end - token->start;
	char *text_token = calloc(size+1, sizeof(char));
	
	if(text_token == NULL)
	{
		printf("error when reading token \n");
		exit(0);
	}
	strncpy(text_token, text+token->start, size);
	return text_token;
}


/*
* Extract the features of the subclass "Account".
*/
void
_access_personal_info_ACCOUNT(char *feature, int offset, double *feature_vector, FeatureSet *features)
{
	
	if(strcmp(feature, "android.accounts.IAccountManager.TRANSACTION_getAccountsByFeatures()") == 0) feature_vector[offset] += lowlevel_behaviour;
	else if(strcmp(feature, "android.accounts.IAccountManager.TRANSACTION_getAccounts()") == 0) feature_vector[offset+1] += lowlevel_behaviour;

	feature_vector[_size] += lowlevel_behaviour;	
}


/*
* Extract the features of the subclass "SMS"
*/
void
_access_personal_info_SMS(char *feature, int offset, double *feature_vector, FeatureSet *features)
{

	if(strcmp(feature, "android.provider.Telephony.SMS_RECEIVED") == 0) feature_vector[offset] += lowlevel_behaviour;
	else if(strcmp(feature, "BINDER") == 0) feature_vector[offset+1] += lowlevel_behaviour;
	
	feature_vector[_size] += lowlevel_behaviour;
}

/*
* Extract the features of the subclass "PHONE"
*/
void
_access_personal_info_PHONE(char *feature, int offset, double *feature_vector, FeatureSet *features)
{
	if(strcmp(feature, "android.app.IActivityManager.GET_CONTENT_PROVIDER_TRANSACTION()") == 0) feature_vector[offset] += lowlevel_behaviour;
	else if(strcmp(feature, "com.android.internal.telephony.IPhoneSubInfo.getVoiceMailNumber()()") == 0) feature_vector[offset+1] += lowlevel_behaviour;
       	else if(strcmp(feature, "com.android.internal.telephony.IPhoneSubInfo.getSubscriberId()()") == 0) feature_vector[offset+2] += lowlevel_behaviour;
        else if(strcmp(feature, "com.android.internal.telephony.IPhoneSubInfo.getIccSerialNumber()()") == 0) feature_vector[offset+3] += lowlevel_behaviour;
        else if(strcmp(feature, "com.android.internal.telephony.ITelephony.TRANSACTION_getActivePhoneType()") == 0) feature_vector[offset+4] += lowlevel_behaviour;
        else if(strcmp(feature, "com.android.internal.telephony.ITelephony.TRANSACTION_getNetworkType()") == 0) feature_vector[offset+5] += lowlevel_behaviour;
	else if(strcmp(feature, "com.android.internal.telephony.IPhoneSubInfo.getLine1Number()()") == 0) feature_vector[offset+6] += lowlevel_behaviour;
        else if(strcmp(feature, "com.android.internal.telephony.IPhoneSubInfo.getDeviceId()()") == 0) feature_vector[offset+7] += lowlevel_behaviour;
        else if(strcmp(feature, "com.android.internal.telephony.IPhoneSubInfo.getDeviceSvn()()") == 0) feature_vector[offset+8] += lowlevel_behaviour;
	else
	{
		printf("Other... \n");
	}
	
	feature_vector[_size] += lowlevel_behaviour;
}

/*
* Extract the feautres of the subclass "API_CALL"
*/
void
_access_personal_info_API_CALL(char *feature, int offset, double *feature_vector, FeatureSet *features)
{
	if(strcmp(feature, "android.location.ILocationManager.TRANSACTION_getProviders()") == 0) feature_vector[offset] += lowlevel_behaviour;
	else if(strcmp(feature, "android.location.ILocationManager.TRANSACTION_getProviderInfo()") == 0) feature_vector[offset+1] = lowlevel_behaviour;
	else if(strcmp(feature, "android.location.ILocationManager.TRANSACTION_requestLocationUpdates()") == 0) feature_vector[offset+2] += lowlevel_behaviour;
	else if(strcmp(feature, "android.location.ILocationManager.TRANSACTION_getLastKnownLocation()") == 0) feature_vector[offset+3] += lowlevel_behaviour;
	else if(strcmp(feature, "com.android.internal.telephony.ITelephony.TRANSACTION_getCellLocation()") == 0) feature_vector[offset+4] += lowlevel_behaviour;
	else if(strcmp(feature, "android.location.ILocationManager.TRANSACTION_getAllProviders()") == 0) feature_vector[offset+5] += lowlevel_behaviour; 
	else{
	printf("NOANOANAONAOANOANOAN\n");
	printf("feature: %s \n", feature);
	}
	feature_vector[_size] += lowlevel_behaviour;
//	printf("feature: %s \n", feature);
}

/*
* Extract the features of the subclass "Contacts"
*/
void
_access_personal_info_CONTACTS(char *feature, int offset, double *feature_vector, FeatureSet *features)
{
	if(strcmp(feature, "android.app.IActivityManager.GET_CONTENT_PROVIDER_TRANSACTION()") == 0) feature_vector[offset] += lowlevel_behaviour;
	else
	{
		printf("naASDKFH ASDKFJ AHSDKF HASDFLK SAHDFnanananosadhasd: %s \n", feature);
	}
	feature_vector[_size] += lowlevel_behaviour;
}

void
_network(char *feature, int offset, double *feature_vector, FeatureSet *features)
{

	if(strcmp(feature, "DNS") == 0) feature_vector[offset] += lowlevel_behaviour;
	else if(strcmp(feature, "HTTP") == 0) feature_vector[offset+1] += lowlevel_behaviour;
	else{ feature_vector[offset+2] += lowlevel_behaviour; }
	
	feature_vector[0] += lowlevel_behaviour;
}

char *
_get_executable(char *blob)
{
	char *exec = strchr(blob, ':');
	exec += 3;

	char *comma = strchr(exec, ',');
	int difference = comma-exec-1;
	exec[difference] = '\0';
	char *executed_file = malloc(difference+1);
	strcpy(executed_file, exec);

	return executed_file;	
}

char *
_get_filename(char *blob)
{
	char *last_occur = strrchr(blob, '/');
	if(last_occur == NULL)
	{
		return blob;
	}
	last_occur++;
	int len = strlen(last_occur);
	if(len > 0)
	{	
		char *file_returner = calloc(len-6, sizeof(char));
		strncpy(file_returner, last_occur, len-7);
		file_returner[len-7] = '\0';	
		return file_returner;
	}
	else
	{
		printf("end NULL \n");
		return NULL;
	}
	
}

/*
*
*/
FeatureSet *
extractFeatures(char *content, hashtable_t *family_dic)
{
	double *feature_vector = malloc(sizeof(double)*_feature_vector_size);
	hashtable_t *procnames = create_dictionary(10);
	hashtable_t *filenames = create_dictionary(10);
	hashtable_t *execnames = create_dictionary(10);


	int m=0;
	for(; m<_feature_vector_size; m++)
	{
		feature_vector[m] = 0.0;
	}
	jsmntok_t *tokens_o = getTokens(content);	
	int i = 0;
	int size = tokens_o[i].end;
	FeatureSet *features = calloc(1, sizeof(FeatureSet));
	char *tmp = NULL;
	while((tokens_o[i].end+2) != size)
	{
		if(tokens_o[i].start != tokens_o[i].end)
		{
		 	tmp = printToken(content, &tokens_o[i]);
			if(tmp == NULL)
			{
				printf("it is NULL \n");
				i++;
				continue;
			}
			if(use_procnames){
				if(strcmp(tmp, "procname")==0)
				{
					char *procname = printToken(content, &tokens_o[i+1]);
						//printf("procname: %s\n", procname);
						int procname_count = hashtable_get_value(procnames, procname);
						if(procname_count == 0)
						{	
						//	printf("its zero\n");
							feature_vector[_proccount] += 1.0;
						}
						procname_count++;
						put_hash(procnames, procname, procname_count);
					//	put_hash(family_dic, procname, 5);
						feature_vector[_size] += 1.0;
				}
			}
			if((strcmp(tmp,"class")) == 0)
			{
				char *class = printToken(content, &tokens_o[i+1]);

				if(strcmp(class, "FS ACCESS") == 0)
				{
					feature_vector[_FS] += highlevel_behaviour;
					feature_vector[_size] += highlevel_behaviour;
					if(use_filenames || use_file_alpha)
					{
						char *filename = printToken(content, &tokens_o[i+8]);
						char *real_file_name = _get_filename(filename);	
						if(strcmp(filename, real_file_name) == 0)
						{
							free(filename);
						}
						else
						{
							int filename_count = hashtable_get_value(filenames, real_file_name);
							if(filename_count == 0) feature_vector[_filecount]++;
							filename_count++;
							put_hash(filenames, real_file_name, filename_count);
							put_hash(family_dic, real_file_name, 5);
							feature_vector[_size]+= 1.0;
						
							free(filename);
						}
					}
				}
				else if(strcmp(class, "ACCESS PERSONAL INFO") == 0)
				{
					feature_vector[_personal] += highlevel_behaviour;
					feature_vector[_size] += highlevel_behaviour;

					char *subclass = printToken(content, &tokens_o[i+3]);
					if(strcmp(subclass, "PHONE") == 0)
					{
						feature_vector[_size] += midlevel_behaviour;
						feature_vector[_phone] += midlevel_behaviour;
			
						char *method = printToken(content, &tokens_o[i+8]);
						_access_personal_info_PHONE(method, _phone+1, feature_vector, features);
						free(method);
					}
					
					else if(strcmp(subclass, "SMS") == 0) 
					{
						feature_vector[_size] += midlevel_behaviour;
						feature_vector[_sms] += midlevel_behaviour;
						 
						char *method = printToken(content, &tokens_o[i+10]);
						_access_personal_info_SMS(method, _sms+1, feature_vector, features);
						free(method);
					}
					
					else if(strcmp(subclass, "CALL") == 0)
					{
						feature_vector[_call] += midlevel_behaviour;
						feature_vector[_size] += midlevel_behaviour; 
					}
			
					else if(strcmp(subclass, "ACCOUNT") == 0)
					{
						feature_vector[_account] += midlevel_behaviour;
						feature_vector[_size] += midlevel_behaviour;
		
						char *method = printToken(content, &tokens_o[i+8]);
						_access_personal_info_ACCOUNT(method, _account+1, feature_vector, features);
						free(method);
					}
					else if(strcmp(subclass, "LOCATION") == 0)
					{
						feature_vector[_location] += midlevel_behaviour;
						feature_vector[_size] += midlevel_behaviour;

						char *API_CALL = printToken(content, &tokens_o[i+8]);
						_access_personal_info_API_CALL(API_CALL, _location+1, feature_vector, features);
						free(API_CALL);
					}
					else if(strcmp(subclass, "CONTACTS") == 0)
					{
						feature_vector[_contacts] += midlevel_behaviour;
						feature_vector[_size] += midlevel_behaviour;
						char *API_CALL = printToken(content, &tokens_o[i+8]);
						_access_personal_info_CONTACTS(API_CALL, _contacts+1, feature_vector, features);
						free(API_CALL);
					}
					else
					{
						printf("Subclass identified which is not listed: %s \n", subclass);
					}
					
					free(subclass);
				}
				else if(strcmp(class, "NETWORK ACCESS") == 0)
				{
					feature_vector[_networkk] += highlevel_behaviour;
					feature_vector[_size] += highlevel_behaviour;
					
					char *subclass = printToken(content, &tokens_o[i+3]);
					_network(subclass, _networkk+1, feature_vector, features);
					free(subclass);
				}
				else if(strcmp(class, "EXECUTE") == 0)
				{
					feature_vector[_size] += highlevel_behaviour;
					feature_vector[_execute] += highlevel_behaviour;
					
					char *subclass = printToken(content, &tokens_o[i+3]);
					
					if(strcmp("INSTALL APK", subclass) == 0)
					{
						feature_vector[_execute+1] += lowlevel_behaviour;
						feature_vector[_size] += lowlevel_behaviour;
					}
					else if(strcmp("PRIVILEGE ESCALATION", subclass) == 0)
					{
						feature_vector[_execute+2] += lowlevel_behaviour;
						feature_vector[_size] += lowlevel_behaviour;
					}
					else if(strcmp("SHELL", subclass) == 0)
					{
						feature_vector[_execute+3] += lowlevel_behaviour;
						feature_vector[_size] += lowlevel_behaviour;
					}
					else 
					{
						feature_vector[_execute+4] += lowlevel_behaviour;
						feature_vector[_size] += lowlevel_behaviour; 
					}
					if(use_execnames || use_exec_alpha)
					{
						char *executed_name = printToken(content, &tokens_o[i+10]);
						char *executed_file = _get_executable(executed_name);
						int execcount = hashtable_get_value(execnames, executed_name);
						if(execcount == 0)
						{
							feature_vector[_execcount]++;
						}
						execcount++;
						put_hash(execnames, executed_file, execcount);
						
						free(executed_name);
						feature_vector[_size] += 1.0;
					}
				}
				else if(strcmp(class, "PHONECALL") == 0)
				{
					//printf("phonecall\n");
					feature_vector[_MakeCall] += highlevel_behaviour;
					feature_vector[_size] += highlevel_behaviour;
				}
				else if(strcmp(class, "SMS SEND") == 0)
				{
					//printf(" sms send \n");
					feature_vector[_SMS_send] += highlevel_behaviour;
					feature_vector[_size] += highlevel_behaviour;
				}
				free(class);
			}
			free(tmp);
		}
		i++;
	}	

	/* clean up */
	free(tokens_o);

	features->feature_vector = feature_vector;
	features->procnames = procnames;
	features->filenames = filenames;
	features->execnames = execnames;
/*
	// normalize 
	int j;
	double accumulated = 0.0;
	
	if(feature_vector[_size] > 0){
	for(j=1; j<_proccount; j++)
	{
		feature_vector[j] /= feature_vector[0];
		accumulated += feature_vector[j];	
//		printf("%0.2f  ", feature_vector[j]);
	}
//	printf("\n");
	}
//	printf("accumulated : %f \n", accumulated);
//	printf("\n");*/
	return features;
}

Family *
parse(char *dir)
{
	Family *Root = malloc(sizeof(Family));	
	Root->name = "root";
	
	hashtable_t *family_names = create_dictionary(20);	

	listFiles(dir, Root);
	
	Family *tmpFam = Root;
	int i = 0;
	int counter = 0;	
	while(tmpFam != NULL){
//		printf("tmpFam name: %s \n", tmpFam->name);
			
		if(tmpFam->instances != NULL){
//			printf("after tmpFam->instanec \n");
			Instance *tmp = tmpFam->instances;			
			hashtable_t *familis = create_dictionary(20);
			
			// extract features for an individual sample 	
			while(tmp != NULL){
				char *output = readFile(tmp->path);
				FeatureSet *features = extractFeatures(output, familis);
				if(features->feature_vector[_size] > 0)
				{
					//printf("%s\t", tmp->path);
					//printf("%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\t%0.4f\n", features->feature_vector[_personal], features->feature_vector[_networkk], features->feature_vector[_FS], features->feature_vector[_execute], features->feature_vector[_SMS_send], features->feature_vector[_MakeCall]);

				//printf(" ACP: %0.2f\t FS %0.2f\t NET %0.2f\t EXE %.2f\n", features->feature_vector[_personal], features->feature_vector[_FS], features->feature_vector[_networkk], features->feature_vector[_execute]);
					counter++;
				}
				i++;
				/*if(features->feature_vector[0] >= 0) printf("Filename: %-.45s\t%0.2f \t FS: %.2f \t ACP: %.2f \t NET: %.2f \t EXE: %.2f SMS: %.2f \t ACC: %.2f \t CAL: %.2f PRC: %.2f \t FIL: %.2f \t %d\n", 
					tmp->path, features->feature_vector[0], features->feature_vector[_FS], 
					features->feature_vector[_personal], features->feature_vector[_networkk], features->feature_vector[_execute], features->feature_vector[_sms], features->feature_vector[_account], features->feature_vector[_call], features->feature_vector[_proccount], features->feature_vector[_filecount],  i++);
			*/
				free(output);
			        tmp->feature_set = features;	
				tmp = tmp->next;
			}
			key_w *fam_key = familis->keys;
			for(; fam_key != NULL; fam_key = fam_key->next)
			{
				int val = hashtable_get_value(family_names, fam_key->key);
				val++;
				put_hash(family_names, fam_key->key, val);
			}
		}
		tmpFam = tmpFam->next;
	}
	printf("Entire number of files parsed %d \n", i);
	printf("files with more than one feature: %d\n", counter);
	printf("files without any feature: %d\n",i-counter);
	
	return Root;
}

char *
readFile(char *filename)
{
	int fp, numread, i;
	char *content;
	char *buffer = calloc(READ_SIZE+1, sizeof(char));	
	

	content = calloc(READ_SIZE+1, sizeof(char));
	if(content == NULL) printf("NULLLLLL \n");
	
	fp = open(filename, O_RDONLY);
	i=1;
	for(;;i++){
		numread = read(fp, buffer, READ_SIZE);
		if(numread == -1){
			printf("error opening file\n");
			exit(0);
		}
		if(numread < READ_SIZE){
			buffer[numread] = '\0';
			if(i != 1)
			{
				char *newcontent = realloc(content, (((READ_SIZE*(i-1)) + numread)+1) * sizeof(char));
				if(!newcontent)
				{
					printf("Failed to reallocate in readfile\n");
					exit(0);
				}
				strcat(newcontent, buffer);
				
				free(buffer);			
				close(fp);
				return newcontent;
			}
			strcat(content, buffer);
			break;
		}
		buffer[READ_SIZE] = '\0';
		char *newcontent2 = realloc(content, (READ_SIZE*i)+1 * sizeof(char));
		if(!newcontent2)
		{
			printf("error when reallocating newcontent \n");
			exit(0);
		}
		strcat(newcontent2, buffer);
	
		content = newcontent2;
	}
	close(fp);
	return content;
}

int
listFiles(char *dirpath, Family *head)
{
        DIR *dirp;
        struct dirent *dp;
        char *temp_path;
        struct stat statbuf;

        temp_path = calloc(1048, sizeof(char));

        dirp = opendir(dirpath);
        if(dirp==NULL){
                printf("Failed to open dir %s \n", dirpath);
                return -1;
        }
        for(;;){
                dp = readdir(dirp);
                if(dp == NULL) break;

                if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                        continue;       /* Skip . and .. */
               
		strcpy(temp_path, dirpath);
		strcat(temp_path, "/");
		strcat(temp_path, dp->d_name); 
                
		if(stat(temp_path, &statbuf) == -1){
                        printf("error: %s\n", strerror(errno));
                        exit(0);
                }
	
                if(S_ISDIR(statbuf.st_mode)){
			Family *tmp_fam;
			tmp_fam = malloc(sizeof(Family));
		
			tmp_fam->name = calloc(strlen(dp->d_name)+1, sizeof(char));
			strcpy(tmp_fam->name, dp->d_name);
			

			tmp_fam->next = head->next;
			head->next = tmp_fam;
			tmp_fam->instances = NULL;
                        listFiles(temp_path, tmp_fam);
                }
                else{
			char *path = calloc(strlen(temp_path)+1, sizeof(char));
			strcpy(path, temp_path);
			
			Instance *instance = malloc(sizeof(Instance));
			instance->path = path;
			
			instance->family_name = calloc(strlen(head->name)+1, sizeof(char));
			strcpy(instance->family_name, head->name);
			
			instance->next = head->instances;
			head->instances = instance;
                }
        }
	
	if(closedir(dirp) == -1)
		printf("error when closing dir\n");
	free(temp_path); 
        return 0;
}

int FamilySize(Family *root, double min_nr_of_features, double max_nr_of_features)
{
	int size = 0;
	while(root != NULL){
		if(root->instances != NULL){
			Instance *tmp = root->instances;
			while(tmp != NULL){
				if(tmp->feature_set->feature_vector[0] >= min_nr_of_features && tmp->feature_set->feature_vector[0] <= max_nr_of_features)
					size++;
				tmp = tmp->next;
			}
		}
		root = root->next;
	}
	return size;	
}

hashtable_t * 
ListFamilyNames(Family *root, double min_nr_of_features)
{
	hashtable_t *hashtable = create_dictionary(10);
	
	while(root != NULL)
	{
		if(root->instances != NULL)
		{
			Instance *tmp = root->instances;
			int tmp_size = 0;
			
			for(; tmp != NULL; tmp = tmp->next)
			{
				if(tmp->feature_set->feature_vector[0] >= min_nr_of_features)
					tmp_size++;
			}

			if(tmp_size > 0) put_hash(hashtable, root->name, tmp_size);
		}
		root = root->next;
		
	}
	return hashtable;
}
