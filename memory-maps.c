#ifndef __KERNEL__
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#if linux
#include <malloc.h>
#endif
#endif

#include <aquarium-driller/system.h>
#include <aquarium-driller/random.h>
#include <aquarium-driller/memory-maps.h>

static struct memory_maps *memory_maps;

char *page_zeros;
char *page_0xff;
char *page_rand;
void **page_allocs;


static struct map * alloc_map(void)
{
	struct map *newmap;

	newmap = ALLOC_MAX4M_REGULAR(sizeof(struct map));

	if (!newmap) {
		return NULL;
	}

	memset(newmap, 0, sizeof(struct map));

	newmap->size = 0;
	newmap->ptr = NULL;
	newmap->name = NULL;
	newmap->free_op = NULL;

	newmap->prev = NULL;
	newmap->next = NULL;

	return newmap;
}

static void append_map(struct map *map)
{
	if (memory_maps == NULL)
		return;

	/* First element inserted the list */
	if ((memory_maps->head == NULL) && (memory_maps->tail == NULL)) {
		memory_maps->head = map;
		memory_maps->tail = map;
	}

	/* Give previously last element a subsequent element */
	memory_maps->tail->next = map;

	map->prev = memory_maps->tail;
	map->next = NULL;

	memory_maps->tail = map;
	memory_maps->num_mappings++;
}

static void dump_maps(void)
{
	struct map *tmpmap = NULL;
	unsigned int j;

	if (memory_maps == NULL)
		return;

	tmpmap = memory_maps->head;
	for (j = 0; j < memory_maps->num_mappings; j++) {
		tmpmap = tmpmap->next;
	}
}

static void * append_random_map(struct map *map, int prot, const char *name)
{
	struct map *tmpmap = map;
	size_t size = 0;

	if (tmpmap == NULL)
		tmpmap = alloc_map();

	if (tmpmap == NULL) {
		printk (KERN_ALERT "AD ERROR: Could not allocate tmpmap\n");
		return NULL;
	}

	/* Pick a random sized mmap. */

	switch (RAND(sizeof(int)) % 4) {
	case 0:	size = PAGE_SIZE;
		break;
	case 1:	size = 1024 * 1024;
		break;
	case 2:	size = 2 * (1024 * 1024);
		break;
	case 3:	size = 4 * (1024 * 1024);
		break;
	default:
		size = PAGE_SIZE * 2;
		break;
	}

	/* PAGE_SIZE * 2, so we have a guard page afterwards.
	 * This is necessary for when we want to test page boundaries.
	 * see end of _get_address() for details.
	 */
	if (size <= (PAGE_SIZE * 2))
		size *= 2;

	if (size > MAX_KMALLOC_SIZE)
		tmpmap->ptr = ALLOC_UNLIMITED_VIRT(size);
	else
		tmpmap->ptr = ALLOC_MAX4M_REGULAR(size);

	if (tmpmap->ptr == NULL) {
		printk (KERN_ALERT "AD ERROR: Could not allocate tmpmap->ptr (%lx)\n", size);
		return NULL;
	}

	tmpmap->size = size;
	tmpmap->name = ALLOC_MAX4M_REGULAR(80);
	if (!tmpmap->name) {
		return NULL;
	}
	strncpy (tmpmap->name, name, 80 - 1);

	if (size > MAX_KMALLOC_SIZE)
		tmpmap->free_op = LARGE_FREE_FPTR;
	else
		tmpmap->free_op = SMALL_FREE_FPTR;

	size = 0;
	while (size < tmpmap->size) {
		generate_random_page((char *) tmpmap->ptr + size);
		size += PAGE_SIZE;
	}

	if (prot & PROT_X)
		PROT_PAGE_X(tmpmap->ptr, size / PAGE_SIZE);
	if (!(prot & PROT_W))
		PROT_PAGE_RO(tmpmap->ptr, size / PAGE_SIZE);

	append_map(tmpmap);

	return tmpmap;
}

int setup_maps(void)
{
	struct map *tmpmap;

	memory_maps = ALLOC_MAX4M_REGULAR(sizeof(struct memory_maps));
	if (memory_maps == NULL)
		goto fail;

	memory_maps->head = NULL;
	memory_maps->tail = NULL;
	memory_maps->num_mappings = 0;

	/* First map (head) */
	tmpmap = alloc_map();
	if (tmpmap == NULL)
		goto fail;
	append_map(tmpmap);

	/* Add a set of random zero'ed mappings */
	tmpmap = append_random_map(NULL, PROT_R | PROT_W, "PROT_R | PROT_W");
	if (tmpmap == NULL)
		goto fail;

	tmpmap = append_random_map(NULL, PROT_R, "PROT_R");
	if (tmpmap == NULL)
		goto fail;

	tmpmap = append_random_map(NULL, PROT_W, "PROT_W");
	if (tmpmap == NULL)
		goto fail;

	dump_maps();

	return 0;

fail:
	AD_FAIL = true;
	return -1;
}

size_t get_memory_maps_size(void)
{
	if (memory_maps == NULL)
		return 0;

	return memory_maps->num_mappings;
}

struct map * get_random_map(void)
{
	struct map *tmpmap = NULL;
	unsigned int i, j;

	if (memory_maps == NULL)
		return NULL;

	tmpmap = memory_maps->head;
	i = RAND(sizeof(int)) % memory_maps->num_mappings;
	for (j = 0; j < i; j++)
		tmpmap = tmpmap->next;

	return tmpmap;
}

struct map * get_first_map(void)
{
	if (memory_maps == NULL)
		return NULL;

	return memory_maps->head;
}

struct map * get_last_map(void)
{
	if (memory_maps == NULL)
		return NULL;

	return memory_maps->tail;
}

void destroy_maps(void)
{
	struct map *thismap = NULL,
			   *next = NULL;

	if (memory_maps == NULL)
		return;

	thismap = memory_maps->head;
	while (thismap) {
		thismap->prev = NULL;
		next = thismap->next;

		if (thismap->ptr)
			thismap->free_op(thismap->ptr);
		if (thismap->name)
			FREE_MAX4M_REGULAR(thismap->name);

		FREE_MAX4M_REGULAR(thismap);

		thismap = next;
		memory_maps->head = thismap;
		memory_maps->num_mappings--;
	}
	
	if (memory_maps->num_mappings != 0)
		printk(KERN_ALERT "AD ERROR: internal error while destroying maps\n");
}

void destroy_buffers(void)
{
	unsigned int i;

	if (page_zeros)
		FREE_UNLIMITED_VIRT(page_zeros);
	if (page_0xff)
		FREE_UNLIMITED_VIRT(page_0xff);
	if (page_rand)
		FREE_UNLIMITED_VIRT(page_rand);
	for (i = 0; i < (PAGE_SIZE / sizeof(void *)); i++)
		FREE_MAX4M_REGULAR((void *) page_allocs[i]);
	FREE_UNLIMITED_VIRT(page_allocs);
}


int init_buffers(void)
{
	size_t i;

	if (setup_maps() == -1)
		goto fail;

	page_zeros = ALLOC_UNLIMITED_VIRT(PAGE_ZEROS_SIZE);
	if (!page_zeros) {
		printk (KERN_ALERT "AD ERROR: Could not allocate page_zeros (PAGE_SIZE * 2)\n");
		goto fail;
	}
	memset(page_zeros, 0, PAGE_ZEROS_SIZE);

	page_0xff = ALLOC_UNLIMITED_VIRT(PAGE_FF_SIZE);
	if (!page_0xff) {
		printk (KERN_ALERT "AD ERROR: Could not allocate page_0xff (PAGE_SIZE * 2)\n");
		goto fail;
	}
	memset(page_0xff, 0xff, PAGE_FF_SIZE);


	page_allocs = ALLOC_UNLIMITED_VIRT(PAGE_ALLOC_SIZE);
	if (!page_allocs) {
		printk (KERN_ALERT "AD ERROR: Could not allocate page_allocs (PAGE_SIZE * 2)\n");
		goto fail;
	}

	for (i = 0; i < (PAGE_ALLOC_SIZE / sizeof(void *)); i++) {
		page_allocs[i] = ALLOC_MAX4M_REGULAR(PAGE_SIZE);
		if (page_allocs[i] == NULL) {
			printk (KERN_ALERT "AD ERROR: Could not allocate page_allocs[%zu] (PAGE_SIZE)\n", i);
			goto fail;
		}
		memset(page_allocs[i], 0x41, PAGE_SIZE);
	}

	/* generate_random_page may end up using maps, so has to be last */
	page_rand = ALLOC_UNLIMITED_VIRT(PAGE_RAND_SIZE);
	if (!page_rand) {
		printk (KERN_ALERT "AD ERROR: Could not allocate page_rand (PAGE_SIZE * 2)\n");
		goto fail;
	}

	for (i = 0; i < (PAGE_RAND_SIZE / PAGE_SIZE); i++) {
		generate_random_page((char *) page_rand + (i * PAGE_SIZE));
	}

	i = PAGE_RAND_SIZE % PAGE_SIZE;
	while (i < PAGE_RAND_SIZE)
		page_rand[i++] = RAND(sizeof(uint8_t));


	return 0;

fail:
	AD_FAIL = true;
	return -1;
}

char * get_page_zero(void)
{
	return page_zeros;
}

char * get_page_0xff(void)
{
	return page_0xff;
}

char * get_page_rand(void)
{
	return page_rand;
}

void **get_page_allocs(void)
{
	return page_allocs;
}

