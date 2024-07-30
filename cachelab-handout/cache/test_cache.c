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


int main(){
	test_init_cache();
	test_copy_block();
	test_insert_cache();
	read_and_write();
	return 0;
}
