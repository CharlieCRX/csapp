#include <stddef.h>
// Enumeration for memory operation types
typedef enum {
	LOAD,
	STORE,
	MODIFY,
	UNKNOWN
}OperationType;

// Structure for memeory operation
typedef struct {
	size_t address; //Memory address
	char instruction; //Insrtuction type
	size_t data_size;	//Data size
} MemoryOperation;

// Structure to store cache statistics
typedef struct {
	int hits;
	int misses;
	int evictions;
} CacheStatistics;

// Structure to represent a cache block which contains a pointer to data
typedef struct {
	char *data;
	int data_size;
}CacheBlock;


// Structure to represent a cache line
typedef struct {
	int valid;
	int tag;
	CacheBlock block;
	unsigned long last_access_time; // Time of last access for LRU
}CacheLine;

// Structure to represent a cache set
typedef struct {
	CacheLine *lines;		// Array of cache lines in this set
	int line_nums;			// Number of cache lines in the set
	unsigned long time; // Time counter for LRU
}CacheSet;

// Structure to represent the entire cache
typedef struct {
	CacheSet *sets;
	int num_sets;
	int num_lines_per_set;
	int block_size;
	CacheStatistics cache_log;
}Cache;

// Structure to represent the partition
typedef struct {
	int index;
	int tag;
	int offset;
}AddressPartition;


// Function declarations
void print_help();
void init_cache(Cache *cache, int num_sets, int num_lines_per_set, int block_size);
CacheLine* find_cache_line(Cache *cache, int set_index, int tag);
char* access_cache_word(CacheLine *line, size_t offset);
void copy_block(CacheBlock memory_block, CacheBlock cache_block);
void insert_cache_line(Cache *cache, int set_index, int tag, CacheBlock memory_block, int verbose);
void free_cache(Cache *cache);
AddressPartition get_address_partition(Cache *cache, size_t address);
void handle_memory_operation(Cache *cache, MemoryOperation operation, int verbose); 
void handle_memory_operation_without_size(Cache *cache, MemoryOperation operation, int verbose);
int parse_line(const char *line, MemoryOperation *operation);
int convert_tracefile_to_memory_operation(const char *filename, MemoryOperation **operations);
