#include <stdio.h>
#include "cachelab.h"
#include "cache/cache.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>

int main(int argc, char *argv[])
{
	char opt;
	int s,E,b;
	int verbose = 0;
	char *filename = (char*) malloc (128 * sizeof(char));

	while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				exit(0);
			case 'v':
				verbose = 1;
				break;

			case 's':
				s = atoi(optarg);
				break;

			case 'E':
				E = atoi(optarg);
				break;

			case 'b':
				b = atoi(optarg);
				break;

			case 't':
				strcpy(filename, optarg);
				break;
			default:
				print_help();
				exit(0);
		}
	}
	int num_sets = 1 << s;
	int num_lines_per_set = E;
	int block_size = 1 << b;

	Cache cache;
	init_cache(&cache, num_sets, num_lines_per_set, block_size);

	MemoryOperation *operations;
	int num_operations = convert_tracefile_to_memory_operation(filename, &operations);
 
  if (num_operations > 0) {
    for (int i = 0; i < num_operations; i++) {
      if (verbose) {
        printf("%c %zx,%zu ", operations[i].instruction, operations[i].address, operations[i].data_size);
      }

      handle_memory_operation(&cache, operations[i], verbose);
      if (verbose) printf("\n");
    }
    free(operations);
    free_cache(&cache);
		printSummary(cache.cache_log.hits, cache.cache_log.misses, cache.cache_log.evictions);
	} else {
		print_help();
	}
	return 0;
}
