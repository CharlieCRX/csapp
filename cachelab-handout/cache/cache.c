#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include <limits.h>

/*
	Initializes the cache with the specified number of sets, lines per set, and block size.
	(S)num_sets: Number of sets.
	(E)num_lines_per_set: Number of lines per set.
	(B)block_size: Block size(bytes)
*/
void init_cache(Cache *cache, int num_sets, int num_lines_per_set, int block_size){

	//initialize the parameters of cache
	cache->num_sets = num_sets;
	cache->num_lines_per_set = num_lines_per_set;
	cache->block_size = block_size;
	cache->sets = (CacheSet *)malloc(num_sets * sizeof(CacheSet));
	cache->cache_log.hits = 0;
	cache->cache_log.misses = 0;
	cache->cache_log.evictions = 0;

	//organize the cache set
	for(int i = 0; i < num_sets; i++){

		cache->sets[i].line_nums = num_lines_per_set;
		cache->sets[i].lines = (CacheLine *) malloc(num_lines_per_set * sizeof(CacheLine));
		cache->sets[i].time = 0; //LRU time

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
			target_set.lines[i].last_access_time = ++target_set.time; //Update access time
			cache->cache_log.hits ++;	// Cache hit
			return (target_set.lines + i);
		}
	}

	//Cache miss
	cache->cache_log.misses++;
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
	unsigned long min_time = ULONG_MAX;
	int lru_index = 0;	// Index of the least recently used cache line

	for (int i = 0; i < target_set.line_nums; i++){
		// If a cache line is invalid, choose it for replacement
		if (!target_set.lines[i].valid) {
			lru_index = i;
			break;
		}
	
		// Otherwise, find the cache line with the samllest last access time
		if (target_set.lines[i].last_access_time < min_time){
			min_time = target_set.lines[i].last_access_time;
			lru_index = i;
		}

	}
		// Cache evictions:If the selected line is valid, we are evicting it.
		if (target_set.lines[lru_index].valid) {
			cache->cache_log.evictions++;
		}

		// Replace the LRU cache line with the new memory block
		CacheLine *line = &target_set.lines[lru_index];
		line->valid = 1;
		line->tag = tag;
		copy_block(memory_block, line->block);
		line->last_access_time = ++target_set.time;
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


/*---------------------- Memory operation part -------------------------------------------*/

// Convert memory address to cache parameters
AddressPartition get_address_partition(Cache *cache, int address) {
	AddressPartition cache_params;
	cache_params.index = (address / cache->block_size) % cache->num_sets;
	cache_params.tag = address / (cache->block_size * cache->num_sets);
	cache_params.offset = address % cache->block_size;
	return cache_params;
}

// Get the operation type
OperationType get_operation_type(const char operation_char) {
	if (operation_char == 'L') {
		return LOAD;
	} else if (operation_char == 'S') {
		return STORE;
	} else if (operation_char == 'M') {
		return MODIFY;
	} else {
		return UNKNOWN;
	}
}

// Function to handle a cache hit
void process_cache_hit(Cache *cache, CacheLine *line, char operation, int offset, int size) {
	if (operation == 'M' || operation == 'S') {
		// Write back to cache
		char *data = access_cache_word(line, offset);
		memset(data, 0, size);	// Example modification
	}
}

// Function to handle a cache miss
void process_cache_miss(Cache *cache, char operation, AddressPartition cache_params) {
	CacheBlock memory_block;
	memory_block.data_size = cache->block_size;
	memory_block.data = (char *) malloc(cache->block_size * sizeof(char));

	// Simulate reading from memory (e.g., initialize with zeros)
	memset(memory_block.data, 0, cache->block_size);
	insert_cache_line(cache, cache_params.index, cache_params.tag, memory_block);
	free(memory_block.data);
	if (operation == 'M') {
		CacheLine *line = find_cache_line(cache, cache_params.index, cache_params.tag);
	}

}


void print_cache_params(AddressPartition cache_params) {
	printf("index = %d, tag = %d, offset = %d\n", cache_params.index, cache_params.tag, cache_params.offset);
}
// Function to handle memory operation
void handle_memory_operation(Cache *cache, char operation, int address, int size)	{
	int block_size = cache->block_size;

	// Loop over all blocks in the data to be accessed
	for (int i = 0; i < size; i += block_size) {
		int current_block_size = (size - i < block_size) ? (size - 1) : block_size;
		int current_address = address + i;

		AddressPartition cache_params = get_address_partition(cache, current_address);

		// Check if the block is in the cache
		CacheLine *line = find_cache_line(cache, cache_params.index, cache_params.tag);
		if (line) {
			process_cache_hit(cache, line, operation, cache_params.offset, current_block_size);
		} else {
			process_cache_miss(cache, operation, cache_params);
	}
}

// Get operations from tracefile
MemoryOperation * convert_tracefile_to_memory_operation(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening file");
		return NULL;
	}


