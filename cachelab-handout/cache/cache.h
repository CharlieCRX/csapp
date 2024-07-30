#include <stddef.h>

//Structure to represent a cache block which contains a pointer to data
typedef struct {
	char *data;
	int data_size;
}CacheBlock;


//Structure to represent a cache line
typedef struct {
	int valid;
	int tag;
	CacheBlock block;
}CacheLine;

//Structure to represent a cache set
typedef struct {
	CacheLine *lines;		// Array of cache lines in this set
	int line_nums;			// Number of cache lines in the set
}CacheSet;

//Structure to represent the entire cache
typedef struct {
	CacheSet *sets;
	int num_sets;
	int num_lines_per_set;
	int block_size;
}Cache;


// Function declarations
void init_cache(Cache *cache, int num_sets, int num_lines_per_set, int block_size);
CacheLine* find_cache_line(Cache *cache, int set_index, int tag);
char* access_cache_word(CacheLine *line, size_t offset);
void copy_block(CacheBlock memory_block, CacheBlock cache_block);
void insert_cache_line(Cache *cache, int set_index, int tag, CacheBlock memory_block);
void free_cache(Cache *cache);
