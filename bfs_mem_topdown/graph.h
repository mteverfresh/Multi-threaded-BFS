#ifndef __GRAPH_H__
#define __GRAPH_H__
#include "util.h"
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "wtime.h"
#include <sys/stat.h>

typedef double path_t;
inline off_t fsize(const char *filename) {
	struct stat st; 
	if (stat(filename, &st) == 0)
		return st.st_size;
	return -1; 
}

class graph
{
	public:
		index_t *beg_pos;
		vertex_t *csr;
		path_t *weight;
		index_t vert_count;
		index_t edge_count;
		vertex_t *src_list;
		index_t src_count;

	public:
		graph(){};
		~graph(){};
		graph(const char *beg_file, 
				const char *csr_file,
				const char *weight_file);

		void gen_src(){};
		void groupby(){};
};
#endif
