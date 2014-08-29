#include <stdio.h>
#include <stdlib.h>
#include "priq.h"


pri_queue 
priq_new(int size)
{
	if(size < 4) size = 4;
	
	pri_queue q = malloc(sizeof(pri_queue_t));
	q->buf = malloc(sizeof(q_elem_t) * size);
	q->alloc = size;
	q->n = 1;
	
	return q;
}

void
priq_push(pri_queue q, void *data, double pri)
{
	q_elem_t *b;
	int n, m;

	if(q->n >= q->alloc){
		q->alloc *= 2;
		b = q->buf = realloc(q->buf, sizeof(q_elem_t) * q->alloc);
	}else
		b = q->buf;
	n = q->n++;
	while((m = n/2) && pri > b[m].pri){
		b[n] = b[m];
		n = m;
	}

	b[n].data = data;
	b[n].pri = pri;
}

void *
priq_pop(pri_queue q, double *pri)
{
	void *out;
	if(q->n == 1) return 0;

	q_elem_t *b = q->buf;
	
	out = b[1].data;
	if(pri) *pri = b[1].pri;
	
	/* pull last item to top, then down heap */
	--q->n;
	int n = 1, m;
	
	while((m = n*2) < q->n){
		if(m+1 < q->n && b[m].pri < b[m+1].pri) m++;
		if(b[q->n].pri >= b[m].pri) break;
		b[n] = b[m];
		n = m;
	}
	
	b[n] = b[q->n];
	if(q->n < q->alloc / 2 && q->n >= 16)
		q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(b[0]));

	return out;
}

void 
*priq_top(pri_queue q, double *pri)
{
	if(q->n == 1) return 0;
	if(pri) *pri = q->buf[1].pri;
	return q->buf[1].data;
}

void
priq_clear(pri_queue q)
{
	char *c;
	double p;
	while((c = (char *) priq_pop(q, &p)));
}


void
priq_print(pri_queue q)
{
	int n = q->n;
	printf("size: %d\n", n);
	int i=1;
	for(; i<n; i++)
	{
		int *tmp = (int*)q->buf[i].data;
		printf("| %.2f, %d   ", q->buf[i].pri, *tmp);
	}
	printf("\n");
}

/*
int main()
{
	int i;
	double p;
	char *c, *tasks[] = {
		"test1", "test2", "test3", "test4"
	};
	
	double pri[] = {0.58,0.4,0.9,0.2};

	pri_queue q = priq_new(0);
	for(i=0; i<4; i++){
		priq_push(q, tasks[i], pri[i]);
	}
	
	priq_print(q);
}
*/
