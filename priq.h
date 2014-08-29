typedef struct {
	void * data;
	double pri;
} q_elem_t;

typedef struct {
	q_elem_t *buf;
	int n;
	int alloc;
} pri_queue_t, *pri_queue;


/*	Initialise a new queue. If size=0, size will be set to four */
pri_queue priq_new(int size);

/*	Push the element that *data points to into the queue with priority of pri */
void priq_push(pri_queue q, void *data, double pri);

/*	Pop the top element of the queue and return a void pointer to the
	data of the top element. 
	Set *pri to the priority of the top element */
void * priq_pop(pri_queue q, double *pri);

/* 	Return a void pointer to the data of the top element in the queue.
	Set *pri to the priority of the top element */
void *priq_top(pri_queue q, double *pri);

void priq_clear(pri_queue q);

void priq_print(pri_queue q);
