#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include <limits.h>

void print_help() {
	printf("** A Cache Simulator by Deconx\n");
	printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
	printf("Options:\n");
	printf("-h         Print this help message.\n");
	printf("-v         Optional verbose flag.\n");
	printf("-s <num>   Number of set index bits.\n");
	printf("-E <num>   Number of lines per set.\n");
	printf("-b <num>   Number of block offset bits.\n");
	printf("-t <file>  Trace file.\n\n\n");
	printf("Examples:\n");
	printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
	printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
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
void insert_cache_line(Cache *cache, int set_index, int tag, CacheBlock memory_block, int verbose){
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
			if (verbose) {
				printf("eviction ");
			}
			cache->cache_log.evictions++;
		}

		// Replace the LRU cache line with the new memory block
		CacheLine *line = &target_set.lines[lru_index];
		line->valid = 1;
		line->tag = tag;
		//copy_block(memory_block, line->block);
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
AddressPartition get_address_partition(Cache *cache, size_t address) {
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

/*
	Assume that memory accesses are aligned properly, such that a single
	memory access never crosses block boundaries.
*/
void handle_memory_operation_without_size(Cache *cache, MemoryOperation operation, int verbose) {
	int block_size = cache->block_size;
	AddressPartition cache_params = get_address_partition(cache, operation.address);
	
	// Check if the block is in the cache
	CacheLine *line = find_cache_line(cache, cache_params.index, cache_params.tag);

	// Assume the memory block is aligned properly
	CacheBlock memory_block;
	memory_block.data_size = block_size;
	memory_block.data = (char *) malloc(block_size * sizeof(char));

	if (line && verbose) {
		printf("hit ");
	} else {
		if (verbose) printf("miss ");
		// Miss: first insert the block to cache
		insert_cache_line(cache, cache_params.index, cache_params.tag, memory_block, verbose);
	}

	if (operation.instruction == 'M') {
		CacheLine *line_twice = find_cache_line(cache, cache_params.index, cache_params.tag);
		if (line_twice && verbose) printf("hit ");
	}
}

int find_specific_tag_line(CacheSet *set, int tag) {
	CacheLine *lines = set->lines;
	int line_nums = set->line_nums;
	for (int i = 0; i < line_nums; i++) {
		if(lines[i].valid && lines[i].tag == tag) {
			return i;
		}
	}
	return -1;
}


int find_empty_line(CacheSet *set){
	CacheLine *lines = set->lines;
	int line_nums = set->line_nums;
	for (int i = 0; i < line_nums; i++) {
		if(!lines[i].valid) {
			return i;
		}
	}
	return -1;
}

int find_eviction_line(CacheSet *set) {
	CacheLine *lines = set->lines;
	int line_nums = set->line_nums;
	unsigned long min_time = ULONG_MAX;
	int lru_index = -1;
	for (int i = 0; i < line_nums; i++) {
		if(lines[i].last_access_time < min_time) {
			min_time = lines[i].last_access_time;
			lru_index = i;
		}
	}
	return lru_index;
}

void update_cache_line(Cache *cache, int index, int tag, int verbose) {
	int line_index = find_empty_line(&cache->sets[index]);
	if (line_index == -1) {
		if (verbose) {
			printf("eviction ");
		}
		cache->cache_log.evictions++;
		line_index = find_eviction_line(&cache->sets[index]);
	}
	cache->sets[index].lines[line_index].valid = 1;
	cache->sets[index].lines[line_index].tag = tag;
	cache->sets[index].lines[line_index].last_access_time = ++cache->sets[index].time;
}

void handle_cache_operation(Cache *cache, int index, int tag, int verbose) {
	int line_index = find_specific_tag_line(&cache->sets[index], tag);
	// Miss
	if (line_index == -1) {
		if(verbose) printf("miss ");
		// MISS OPt
		cache->cache_log.misses++;
		update_cache_line(cache, index, tag, verbose);
	} else {
		if (verbose) printf("hit ");
		cache->cache_log.hits++;
	}
}
	

void handle_memory_operation(Cache *cache, MemoryOperation operation, int verbose) {
	AddressPartition cache_params = get_address_partition(cache, operation.address);
	int index = cache_params.index;
	int tag = cache_params.tag;

	if (operation.instruction == 'M') {
		handle_cache_operation(cache, index, tag, verbose);
		handle_cache_operation(cache, index, tag, verbose);
	} else {
		handle_cache_operation(cache, index, tag, verbose);
	}
}



/*
	 Parse a single line of input to extract a memory operation
	 The input line is expected to be in the format: "OPERATION ADDRESS,SIZE",
	 where:
	 - OPERATION is a single character ('L', 'S', or 'M')
	 - ADDRESS is an unsigned integer represented in hexadecimal format
	 - SIZE is a size_t value representing the size of the memory operation
	 Example input line: "L 10,1"
*/
int parse_line(const char *line, MemoryOperation *operation) {
	char verbose;
	char op;
	size_t address;
	size_t size;

	// Use sscanf to parse the line content, expecting the address in hexadecimal format
	if (sscanf(line, "%c%c %zx,%zu", &verbose, &op, &address, &size) != 4) {
		return 0;
	}
	// Omit I instruction
	if (verbose == 'I') return 0;

	// Store the parsed data in the provided MemoryOperation structure
	operation->address = address;
	operation->instruction = op;
	operation->data_size = size;
	return 1;	// Parsing successful
}

/*
	 Read a trace file and converts each line to MemoryOperation.
	 @param filename The name of the trace file to read
	 @param operations Pointer to a pointer to store the dynamically allocated array of MemoryOperation
	 @retuen The number of operations read and stored in the array, or -1 if an error occured.
*/
int convert_tracefile_to_memory_operation(const char *filename, MemoryOperation **operations) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		return -1;
	}

	// Allocate an inital array to store MemoryOperation
	size_t capacity = 10;
	*operations = (MemoryOperation *) malloc (capacity * sizeof(MemoryOperation));
	if(!*operations) {
		perror("Memory allocation error");
		fclose(file);
		return -1;
	}

	char line[256];
	size_t count = 0;

	while (fgets(line, sizeof(line), file)) {
		if (count >= capacity) {
			// Resize the array if the capacity is exceeded
			capacity *= 2;
			MemoryOperation *temp = (MemoryOperation *)realloc(*operations, capacity * sizeof(MemoryOperation));
			if (!temp) {
				perror("Memory reallocation error");
				free(*operations);
				fclose(file);
				return -1;
			}
			*operations = temp;
		}
		MemoryOperation operation;
		if (parse_line(line, &operation)) {
			(*operations)[count++] = operation;
		}
	}

	fclose(file);
	return count;
}
				

