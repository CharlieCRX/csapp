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
	insert_cache_line(&cache, 1, 102, memory_block);

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
	insert_cache_line(&cache, 1, 102, memory_block);
	
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
	init_cache(&cache, num_sets, num_lines_per_set, block_size);
	handle_memory_operation(&cache, 'L',0x10, 1);
	assert(cache.cache_log.hits == 0);
	assert(cache.cache_log.misses== 1);
	assert(cache.cache_log.evictions == 0);

	handle_memory_operation(&cache, 'M',0x20, 1);
	assert(cache.cache_log.hits == 1);
	assert(cache.cache_log.misses== 2);
	assert(cache.cache_log.evictions == 0);
	

	handle_memory_operation(&cache, 'L',0x22, 1);
	assert(cache.cache_log.hits == 2);
	assert(cache.cache_log.misses== 2);
	assert(cache.cache_log.evictions == 0);
	
	

	handle_memory_operation(&cache, 'S',0x18, 1);
	assert(cache.cache_log.hits == 3);
	assert(cache.cache_log.misses== 2);
	assert(cache.cache_log.evictions == 0);

	handle_memory_operation(&cache, 'L',0x110, 1);
	assert(cache.cache_log.hits == 3);
	assert(cache.cache_log.misses== 3);
	assert(cache.cache_log.evictions == 1);

	handle_memory_operation(&cache, 'L',0x210, 1);
	assert(cache.cache_log.hits == 3);
	assert(cache.cache_log.misses== 4);
	assert(cache.cache_log.evictions == 2);


	handle_memory_operation(&cache, 'M',0x12, 1);
	assert(cache.cache_log.hits == 4);
	assert(cache.cache_log.misses== 5);
	assert(cache.cache_log.evictions == 3);
	free_cache(&cache);
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
	insert_cache_line(&cache, 0, 1, memory_block);

	// Insert the second cache line
	strcpy(memory_block.data, "block2");
	insert_cache_line(&cache, 1, 2, memory_block);

	// Insert the third cache line(should cause eviction in the first set)
	strcpy(memory_block.data, "block3");
	insert_cache_line(&cache, 0, 3, memory_block);

	strcpy(memory_block.data, "block4");
	insert_cache_line(&cache, 0, 4, memory_block);
	assert(cache.cache_log.evictions == 2);
	free(memory_block.data);
	free_cache(&cache);
	printf("All insert tests ok!\n");
}

int main(){
	/*
	test_init_cache();
	test_copy_block();
	test_insert_cache();
	read_and_write();
	test_address_partition();
	*/
	test_memory_operation();
	//test_insert_cacheline();
	return 0;
}
