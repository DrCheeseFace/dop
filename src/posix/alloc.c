#include "../internal.h"

#include <sys/mman.h>

Err
alloc_init(alloc_Pool *pool, size_t capacity)
{
	memset(pool, 0, sizeof(*pool));

	pool->pool = mmap(NULL, capacity, PROT_READ | PROT_WRITE,
			  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (pool->pool == MAP_FAILED) {
		return ERR;
	}

	pool->offset = 0;
	pool->capacity = capacity;

	return OK;
}

// @TODO aligned alloc
// @TODO expand on hitting capacity?
void *
alloc_alloc(alloc_Pool *pool, size_t size)
{
	if (pool->offset + size > pool->capacity) {
		return NULL;
	}

	void *alloc_start = pool->pool + pool->offset;
	pool->offset += size;

	return alloc_start;
}

Err
alloc_head_expand(alloc_Pool *pool, size_t size)
{
	if (pool->offset + size > pool->capacity) {
		return ERR;
	}

	pool->offset += size;

	return OK;
}

void
alloc_reset(alloc_Pool *pool)
{
	pool->offset = 0;
}

Err
alloc_free(alloc_Pool *pool)
{
	int err = munmap(pool->pool, pool->capacity);

	if (err) {
		return ERR;
	}
	memset(pool, 0, sizeof(*pool));

	return OK;
}
