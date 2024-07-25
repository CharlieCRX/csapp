#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"

//Initializes the cache with the specified number of sets, lines per set, and block size
void init_cache(Cache *cache, int num_sets, int num_lines_per_set, int block_size){

	//initialize the parameters of cache
	cache->num_sets = num_sets;
	cache->num_lines_per_set = num_lines_per_set;
	cache->block_size = block_size;
	cache->sets = (CacheSet *)malloc(num_sets * sizeof(CacheSet));

	//organize the cache set
	for(int i = 0; i < num_sets; i++){

		cache->sets[i].line_nums = num_lines_per_set;
		cache->sets[i].lines = (CacheLine *) malloc(num_lines_per_set * sizeof(CacheLine));

		//organize the cache line
		for(int j = 0; j < num_lines_per_set; j++){
			cache->sets[i].lines[j].valid = 0;				// Initialize valid bit to 0
			cache->sets[i].lines[j].tag = 0;					// Initialize tag to 0
			cache->sets[i].lines[j].block.data = (char *)malloc(block_size * sizeof(char));
		}
	}
}



