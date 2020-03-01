#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
//#include <list.h>

#define MAX_FILES 4
#define MAX_CHAR 1000000


// Enqueue
void Enqueue(char* line, struct list_node_s** queue_head, struct list_node_s** queue_tail){
	struct list_node_s* tmp_node = NULL;

	tmp_node = malloc(sizeof(struct list_node_s));
	tmp_node -> data =  line;
	tmp_node -> next = NULL;

	# pragma omp critical
	if (*queue_tail == NULL) {
		*queue_head = tmp_node;
		*queue_tail = tmp_node;
	} else {
		(*queue_tail) -> next = tmp_node;
		*queue_tail = tmp_node;
	}

}




// Dequeue
struct list_node_s* Dequeue(struct list_node_s** queue_head, struct list_node_s** queue_tail, int my_rank) {
	struct list_node_s* tmp_node = NULL;
	if (*queue_head == NULL) // empty
		return NULL;
	# pragma omp critical
	{
		if (*queue_head == *queue_tail) // last node
			*queue_tail = (*queue_tail)->next;
			tmp_node = *queue_head;
			*queue_head = (*queue_head)->next;
		}
	return tmp_node;
} /* Dequeue */

// Read file
void Read_file(FILE* file, struct list_node_s** queue_head, struct list_node_s** queue_tail, int my_rank) {
	char* line = malloc(MAX_CHAR*sizeof(char));
	while (fgets(line, MAX_CHAR, file) != NULL) {
		printf("Th %d > read line: %s", my_rank, line);
		Enqueue(line, queue_head, queue_tail);
		line = malloc(MAX_CHAR*sizeof(char));
	}
	fclose(file);
}

void Prod_cons(int prod_count, int cons_count, FILE* files[], int file_count) {
	int thread_count = prod_count + cons_count;
	struct list_node_s* queue_head = NULL; 
	struct list_node_s* queue_tail = NULL;
	int prod_done_count = 0;

	# pragma omp parallel num_threads(thread_count) default(none) \
		shared(file_count, queue_head, queue_tail, files, prod_count, \
			cons_count, prod_done_count)

	{ int my_rank = omp_get_thread_num(), f;
		if (my_rank < prod_count) {
			for (f = my_rank; f < file_count; f += prod_count){
				Read_file(files[f], &queue_head, &queue_tail, my_rank);
			}
			#pragma omp atomic
			prod_done_count++;
		} else {
			struct list_node_s* tmp_node;
			while (queue_head != NULL) {
				tmp_node = Dequeue(&queue_head, &queue_tail, my_rank);
				if (tmp_node != NULL) {
					Tokenize (tmp_node -> data, my_rank);
					free(tmp_node);
				}
			}
			while (queue_head != NULL) {
				tmp_node =  Dequeue(&queue_head, &queue_tail, my_rank);
				if (tmp_node != NULL) {
					Tokenize(tmp_node->data, my_rank);
					free(tmp_node);
				}
			}
		}
	}
		
}


// Main
int main(int argc, char* argv[]) {
	int prod_count, cons_count;
	FILE* files[MAX_FILES];
	int file_count;

	if (argc != 3) Usage(argv[0]);
	prod_count = strtol(argv[1], NULL, 10);
	cons_count = strtol(argv[2], NULL, 10);

	Get_files(files, &file_count);

	printf("prod_count = %d, cons_count = %d, file_count = %d\n", prod_count, cons_count, file_count);

	Prod_cons(prod_count, cons_count, files, file_count);

	return 0;
}















