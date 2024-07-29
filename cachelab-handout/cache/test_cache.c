#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cache.h"

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

	printf("All tests passed!\n");
}

