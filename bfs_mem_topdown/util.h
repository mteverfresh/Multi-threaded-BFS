#ifndef _UTIL_H_
#define _UTIL_H_


typedef long index_t;
typedef int data_t;
typedef signed char lock_t;
typedef long vertex_t;
typedef unsigned char depth_t;

#define META 1
#define DATA 2

#define LOCK(vert, lock) while(!__sync_bool_compare_and_swap(lock+vert,0,-1))
#define UNLOCK(vert, lock) lock[vert]=0
//#define LOCK(vert, lock) 1 
//#define UNLOCK(vert, lock) 0
#endif
