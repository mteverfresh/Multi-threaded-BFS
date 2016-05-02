#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "wtime.h"
#include "graph.h"
#include <omp.h>

#include "util.h"
#define INFTY 255
typedef long index_t;
typedef long vertex_t;
typedef unsigned char depth_t;

int bfs_mem(
	const vertex_t root,
	const char *beg_filename,
	const char *csr_filename,
	const index_t thread_count
){
	graph *ginst = new graph(beg_filename, csr_filename, NULL);
	index_t vert_count = ginst->vert_count;
	index_t edge_count = ginst->edge_count;
	index_t *beg_pos=ginst->beg_pos;
	index_t *csr=ginst->csr;
	depth_t *sa;

	if(posix_memalign((void **)&sa,32,sizeof(depth_t)*vert_count))
		perror("posix_memalign");
	std::cout<<"Degree["<<root<<"] = "<<beg_pos[root+1]-beg_pos[root]<<"\n";
	memset(sa, INFTY, sizeof(depth_t)*vert_count);
	sa[root] = 0;
	index_t *front_comm=new index_t[thread_count];	
	index_t *work_comm=new index_t[thread_count];
	

	double btm= wtime();
	#pragma omp parallel \
	num_threads(thread_count)
	{
		const index_t tid=omp_get_thread_num();
		const index_t step = vert_count/thread_count;
		const index_t vert_beg=tid*step;
		const index_t vert_end=(tid==thread_count-1 ? 
			vert_count:vert_beg+step);
		depth_t level = 0;
			
		while(true)
		{
			double ltm= wtime();
			index_t front_count=0;
			index_t meta_hit=0;
			index_t data_hit=0;
			index_t my_work=0;
			for(vertex_t vert_id=vert_beg; vert_id<vert_end; vert_id++)
			{
				if(sa[vert_id]==level)
				{
					index_t my_beg = beg_pos[vert_id];
					index_t my_end = beg_pos[vert_id+1];

					for(; my_beg<my_end; my_beg++)
					{
						vertex_t nebr=csr[my_beg];
						my_work++;
						if(sa[nebr] == INFTY)
						{
							sa[nebr] = level+1;
							front_count++;
						}
					}
				}
			}
			front_comm[tid]=front_count;
			work_comm[tid]=my_work;

			#pragma omp barrier
			front_count=0;
			meta_hit=0;
			data_hit=0;
			my_work=0;
			for(index_t i=0;i<thread_count;++i)
			{
				front_count += front_comm[i];
				my_work += work_comm[i];
			}
//std::cout<<tid<<" finished\n";
			if(front_count == 0) break;
			if(tid==0) std::cout<<"Level-"<<(int)level<<"-data-meta-total: "
				<<front_count<<" "
				<<wtime() - ltm<<" "
				<<my_work<<"\n";
			#pragma omp barrier

			level ++;
		}
	}
	std::cout<<"Total time: "<<wtime()-btm<<" seconds\n";
  return 0;
}


int main(int args, char **argv)
{
	std::cout<<"/path/to/exe root beg_file csr_file thread_count\n";
	if(args!=5)return -1;
	
	const vertex_t root=atoi(argv[1]);
	const char *beg_file= argv[2];
	const char *csr_file= argv[3];
	const index_t thread_count=atoi(argv[4]);
	
	bfs_mem(root,beg_file,csr_file,thread_count);
}
