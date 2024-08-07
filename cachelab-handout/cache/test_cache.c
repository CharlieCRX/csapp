#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cache.h"
#include <string.h>
void test_init_cache() {
	Cache cache;
	int num_sets = 2;
	int num_lines_per_set = 4;
	int block_size = 8;

	// Initialize the cache
	init_cache(&cache, num_sets, num_lines_per_set, block_size);

	// Check the parameters of cache
	assert(cache.num_sets == num_sets);
	assert(cache.num_lines_per_set == num_lines_per_set);
	assert(cache.block_size == block_size);

	// Check each set and line
	for (int i = 0; i < num_sets; i++) {
		assert(cache.sets[i].line_nums == num_lines_per_set);
		
		for (int j = 0; j < num_lines_per_set; j++){
			assert(cache.sets[i].lines[j].valid == 0);
			assert(cache.sets[i].lines[j].tag == 0);
			assert(cache.sets[i].lines[j].block.data != NULL);
			assert(cache.sets[i].lines[j].block.data_size == block_size);
		}
	}
	//Free the allocated memory for test
	free_cache(&cache);

	printf("Init_cache() tests passed!\n");
}

void test_copy_block(){

	int block_size = 8;
	CacheBlock memory_block;
	memory_block.data_size = block_size;
	memory_block.data = (char *) malloc(block_size * sizeof(char));
	assert(memory_block.data != NULL);

	strcpy(memory_block.data, "imgod!");
	assert(strcmp(memory_block.data, "imgod!") == 0);

	CacheBlock cache_block;
	cache_block.data_size = block_size;
	cache_block.data = (char *) malloc(block_size * sizeof(char));

	copy_block(memory_block, cache_block);
	assert(strcmp(cache_block.data, "imgod!") == 0); 

	free(cache_block.data);
	free(memory_block.data);
	printf("copy_block tests passed!\n");
}


void test_insert_cache() {
	
	Cache cache;
	int num_sets = 2;
	int num_lines_per_set = 4;
	int block_size = 8;

	// Initialize the cache
	init_cache(&cache, num_sets, num_lines_per_set, block_size);

	// Initialize the memory block data and insert to the cache
	CacheBlock memory_block;
	memory_block.data_size = block_size;
	memory_block.data = (char *) malloc(block_size * sizeof(char));
	assert(memory_block.data != NULL);


	strcpy(memory_block.data, "imgod!");
	assert(strcmp(memory_block.data, "imgod!") == 0);
	insert_cache_line(&cache, 1, 102, memory_block, 1);

	assert(cache.sets[1].lines[0].valid == 1);
	assert(cache.sets[1].lines[0].tag == 102);
	printf("insert_cache_line() tests passed!\n");
	free_cache(&cache);
	free(memory_block.data);
}
	


void read_and_write() {

	Cache cache;
	int num_sets = 2;
	int num_lines_per_set = 4;
	int block_size = 8;

	// Initialize the cache
	init_cache(&cache, num_sets, num_lines_per_set, block_size);

	// Initialize the memory block data and insert to the cache
	CacheBlock memory_block;
	memory_block.data_size = block_size;
	memory_block.data = (char *) malloc(block_size * sizeof(char));
	assert(memory_block.data != NULL);


	strcpy(memory_block.data, "imgod!");
	assert(strcmp(memory_block.data, "imgod!") == 0);
	insert_cache_line(&cache, 1, 102, memory_block, 1);
	
	// Find the data from the cache
	CacheLine *target_line = find_cache_line(&cache, 1, 102);
	assert(target_line != NULL);
	assert(target_line->valid == 1);
	assert(target_line->tag == 102);
	assert(target_line->block.data != NULL);

	char *cache_data = access_cache_word(target_line, 1);
	assert(strcmp(cache_data, "mgod!") == 0);
	free_cache(&cache);
	free(memory_block.data);
	printf("All read and write tests passed!\n");
}

void test_address_partition() {

	Cache cache;
	int num_sets = 4;
	int num_lines_per_set = 1;
	int block_size = 2;

	// Initialize the cache
	init_cache(&cache, num_sets, num_lines_per_set, block_size);
	int address = 15;
	AddressPartition cache_params = get_address_partition(&cache, address);
	assert(cache_params.index == 3);
	assert(cache_params.tag == 1);
	assert(cache_params.offset == 1);
	printf("All address partition tests passed!\n");
}
void test_memory_operation() {
	Cache cache;
	int num_sets = 4;
	int num_lines_per_set = 1;
	int block_size = 16;

	// Initialize the cache
	printf("All memory operation tests passed!\n");
}

void test_insert_cacheline() {
	Cache cache;
	int num_sets = 4;
	int num_lines_per_set = 1;
	int block_size = 16;

	// Initialize the cache
	init_cache(&cache, num_sets, num_lines_per_set, block_size);

	//Create a memory block for testing
	CacheBlock memory_block;
	memory_block.data_size = block_size;
	memory_block.data = (char *) malloc(block_size * sizeof(char));
	strcpy(memory_block.data, "block1");

	// Insert the first cache line
	insert_cache_line(&cache, 0, 1, memory_block, 1);

	// Insert the second cache line
	strcpy(memory_block.data, "block2");
	insert_cache_line(&cache, 1, 2, memory_block, 1);

	// Insert the third cache line(should cause eviction in the first set)
	strcpy(memory_block.data, "block3");
	insert_cache_line(&cache, 0, 3, memory_block, 1);

	strcpy(memory_block.data, "block4");
	insert_cache_line(&cache, 0, 4, memory_block, 1);
	assert(cache.cache_log.evictions == 2);
	free(memory_block.data);
	free_cache(&cache);
	printf("All insert tests ok!\n");
}

void test_parse_line() {
	const char *line = " o ff4005b6,5";
	MemoryOperation operation;

	if (parse_line(line, &operation)) {

		printf("Operation: %c, Address: %lx, Size: %zu \n", operation.instruction, operation.address, operation.data_size);

	} else {
		printf("Error parsing line!\n");
	}
}

void print_cache_behavior(int hits, int misses, int evictions) {
	for(int i = 0; i < hits; i++) {
		printf("hit ");
	}
}
void test_convert_tracefile_to_opreation() {
	Cache cache;
	int num_sets = 4;
	int num_lines_per_set = 1;
	int block_size = 4;

	// Initialize the cache
	init_cache(&cache, num_sets, num_lines_per_set, block_size);

	const char *filename = "/home/crx/study/csapp/cachelab-handout/traces/yi3.trace";
	MemoryOperation *operations;
	int num_operations = convert_tracefile_to_memory_operation(filename, &operations);

	if (num_operations > 0) {
		for (int i = 0; i < num_operations; i++) {
			//printf("Operation: %c, Address: %lx, Size: %zu\n", operations[i].instruction, operations[i].address, operations[i].data_size);
			printf("%c %zx,%zu,", operations[i].instruction, operations[i].address, operations[i].data_size);
			int per_hits = cache.cache_log.hits;
			int per_misses = cache.cache_log.misses;
			int per_evictions = cache.cache_log.evictions;

			handle_memory_operation_without_size(&cache, operations[i], 1);
			printf("\n");
		}
		printf("Hits: %d, Misses: %d, Evictions: %d\n", cache.cache_log.hits, cache.cache_log.misses, cache.cache_log.evictions);
		free(operations);
		free_cache(&cache);
	} else {
		printf("Failed to convert trace file to memory operations\n");
	}
}

int main(){
	/*
	test_init_cache();
	test_copy_block();
	test_insert_cache();
	read_and_write();
	test_address_partition();
	*/
	//test_memory_operation();
	//test_insert_cacheline();
	//test_parse_line();
	//printf("unsigned int is %ld size\n", sizeof(size_t));
 	test_convert_tracefile_to_opreation();
	return 0;
}
