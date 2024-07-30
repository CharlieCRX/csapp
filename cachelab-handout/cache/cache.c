#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"

/*
	Initializes the cache with the specified number of sets, lines per set, and block size.
	(S)num_set: Number of sets.
	(E)num_lines_per_set: Number of lines per set.
	(B)block_size: Block size(bytes)
*/
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
			cache->sets[i].lines[j].block.data_size = block_size;
		}
	}
}

/*
	Determine if a copy of the word w is stored in one of the cache lines contained in set i
	A copy of w is contained in the line if and only if the valid bit is set and
	the tag in the cache line matches the tag in the address of w.
*/
CacheLine* find_cache_line(Cache *cache, int set_index, int tag){

	// Set selection
	CacheSet target_set = cache->sets[set_index];

	// Line mactching
	for (int i = 0; i < target_set.line_nums; i++){
		if(target_set.lines[i].valid && target_set.lines[i].tag == tag){
			return (target_set.lines + i);
		}
	}
	return NULL;
}
	
/*
	Read data from the cache block at a specific byte offset.
	size_t offset: The first byte in the desired word.
	In the example, the block offset bits of (0b100) indicate that the copy of w starts at byte 4 in the block.
*/
char* access_cache_word(CacheLine *line, size_t offset){
	if (line->valid) {
		return (line->block.data + offset);
	}
	return NULL;
}

//Copy memory block to cache block
//TODO:Swap the order of two parameters
void copy_block(CacheBlock memory_block, CacheBlock cache_block){
	if(memory_block.data_size == cache_block.data_size){
		int block_size = cache_block.data_size;
		memcpy(cache_block.data, memory_block.data, block_size);
	}
}



/*
	Insert the new block in one of the cache lines of the set indicated by the set index bits.
*/
void insert_cache_line(Cache *cache, int set_index, int tag, CacheBlock memory_block){
	//Set selection
	CacheSet target_set = cache->sets[set_index];
	int index_of_target_line = 0;

	for (int i = 0; i < target_set.line_nums; i++){
		if (!target_set.lines[i].valid) {
			index_of_target_line = i;
			break;
		}
		
		//TODO:maybe exit two equal tags in a set
		if (target_set.lines[i].valid && target_set.lines[i].tag == tag){
			index_of_target_line = i;
			break;
		}

	}
		//Update cache line with new block
		target_set.lines[index_of_target_line].valid = 1;
		target_set.lines[index_of_target_line].tag = tag;
		copy_block(memory_block, target_set.lines[index_of_target_line].block);
}

//Free all allocated memory for the cache
void free_cache(Cache *cache){
	for (int i = 0; i < cache->num_sets; i++){
		for (int j = 0; j < cache->num_lines_per_set; j++){
			free(cache->sets[i].lines[j].block.data); // Free data memory
		}
		free(cache->sets[i].lines);									// Free array of cache lines
	}
	free(cache->sets);														// Free array of cache sets
}
