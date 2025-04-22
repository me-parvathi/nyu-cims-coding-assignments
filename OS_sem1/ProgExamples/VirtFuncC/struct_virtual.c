#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// think of the function table as your virtual functions in C++
/*
 * Base Class for all schedulers
 */

struct scheduler;  // need to forward declare this 

struct function_table {
	int (*get_from_queue) (struct scheduler *obj, int a);
	void (*add_to_queue) (struct scheduler *obj, int a);
	int (*get_quantum) (struct scheduler *obj);
};

struct scheduler {
	struct function_table* funcs;
	int ready_queue;
};

/*----------------- Scheduler Type 1 -------------------*/

struct scheduler_type1 {
	struct scheduler base;
	int quantum;
};

int type1_get_from_queue(struct scheduler* obj, int a) {
	struct scheduler_type1 *sobj = (struct scheduler_type1*)obj; // typo coercion
	printf("%s\n", __FUNCTION__);
	return 1;
}

void type1_add_to_queue(struct scheduler* obj, int a) {
	struct scheduler_type1 *sobj = (struct scheduler_type1*)obj;
	printf("%s Q=%d\n", __FUNCTION__, obj->ready_queue);
}

int type1_get_quantum(struct scheduler* obj) {
	return ((struct scheduler_type1*)obj)->quantum;
}

struct function_table type1_functions = {
	.get_from_queue = &type1_get_from_queue,
	.add_to_queue = &type1_add_to_queue,
	.get_quantum = &type1_get_quantum
};

/*----------------- Scheduler Type 2 -------------------*/

struct scheduler_type2 {
	struct scheduler base;
	int type2stuff;
};

int type2_get_from_queue(struct scheduler* obj, int a) {
	struct scheduler_type2 *sobj = (struct scheduler_type2*)obj; // typo coercion
	printf("%s\n", __FUNCTION__);
	return 1;
}

void type2_add_to_queue(struct scheduler* obj, int a) {
	struct scheduler_type2 *sobj = (struct scheduler_type2*)obj;
	printf("%s a=%d\n", __FUNCTION__, a);
}

int type2_get_quantum(struct scheduler* obj) {
	return ((struct scheduler_type2*)obj)->type2stuff;
}

struct function_table type2_functions = {
	.get_from_queue = &type2_get_from_queue,
	.add_to_queue = &type2_add_to_queue,
	.get_quantum = &type2_get_quantum
};

// initialize an instance 
struct scheduler_type2 type2_scheduler =
{
	.base = { .funcs = &type2_functions, .ready_queue = -2 },
	.type2stuff = -100
};

/*-------------------------------------------------------------*/

struct scheduler* sched;  // that's the only object we use in global algo
struct scheduler_type1 *type1_scheduler; // this is specialized


int main(int argc, char** argv) 
{
	int c;
	int schedtype = 0;
	
	// proper way to parse arguments
	while ((c = getopt(argc,argv,"s:")) != -1 )
	{
		switch(c) {
		case 's':
			schedtype = atoi(optarg);
			break;
		}
	}

	switch (schedtype) {
	case 1:	
	    type1_scheduler = (struct scheduler_type1*)malloc(sizeof(struct scheduler_type1));
	    type1_scheduler->base.funcs = &type1_functions;
	    type1_scheduler->base.ready_queue = -1; 
	    type1_scheduler->quantum = 1000;
	    sched = (struct scheduler*)type1_scheduler;
	    break;
	case 2:
	    sched = (struct scheduler*)&type2_scheduler;
	    break;
	default:
	    printf("At least specify a valid scheduler\n");
	    exit(1);
	}

	sched->funcs->get_from_queue(sched, 1);
	sched->funcs->add_to_queue(sched, 50);
	printf("Quantum: %d\n", sched->funcs->get_quantum(sched));

	return 1;
}
