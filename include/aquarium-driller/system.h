#ifndef	_SYSTEM_H
#define _SYSTEM_H

#define NUM_ONLINE_CPUS 8
#define KERNEL_ADDR 0xffffffff81000000UL 
#define MODULE_ADDR 0xffffffffa0000000UL
#define VDSO_ADDR   0xffffffffff600000UL

#include <stddef.h>

#ifndef __KERNEL__
#include <stdint.h>
#include <stdbool.h>
#define PAGE_SHIFT  		12
#define PAGE_SIZE			(1 << PAGE_SHIFT)
#define PAGE_MASK  			(~(PAGE_SIZE-1))
#define PAGE_OFFSET			(0)
#define PAGE_ALIGN(addr)	(((addr) + PAGE_SIZE - 1) & PAGE_MASK)
#else
#include <asm/page.h>
#include <linux/slab.h>
#include <asm/cacheflush.h>
#endif

extern bool ad_fail;
#define AD_FAIL ad_fail
#define AD_INIT_DRILLER(x) init_driller(x)
#define AD_EXIT_DRILLER exit_driller()

#define PAGE_ZEROS_SIZE PAGE_SIZE * 2
#define PAGE_FF_SIZE    PAGE_SIZE * 2
#define PAGE_RAND_SIZE  PAGE_SIZE * 2
#define PAGE_ALLOC_SIZE PAGE_SIZE * 2

#define regenerate_random_page(page)									\
({																		\
	size_t i;															\
	if (page != NULL) {													\
		for (i = 0; i < (PAGE_RAND_SIZE / PAGE_SIZE); i++)				\
			generate_random_page((char *) page_rand + (i * PAGE_SIZE));	\
		i = PAGE_RAND_SIZE % PAGE_SIZE;									\
		while (i < PAGE_RAND_SIZE)										\
			page_rand[i++] = RAND(sizeof(uint8_t));						\
	}																	\
})

extern char *page_zeros;
extern char *page_0xff;
extern char *page_rand;
extern void **page_allocs;

int			init_driller(uint64_t user_seed);
void		exit_driller(void);
void		generate_random_page(char *page);
int			setup_maps(void);
void		destroy_maps(void);
void		destroy_buffers(void);
struct map	* get_random_map(void);
int			init_buffers(void);
char		* get_page_zero(void);
char		* get_page_0xff(void);
char		* get_page_rand(void);
void		**get_page_allocs(void);
void		dump_memory_range(void *memory, size_t length,
						unsigned char truncate);

/* variable generation */
void		variable_generation(void);

/* memory allocation */
enum page_prot {
	PROT_R = 1 << 0,
	PROT_W = 1 << 1,
	PROT_X = 1 << 2
};

struct map {
	struct map *prev;
	struct map *next;
	char *ptr;
	char *name;
	size_t size;
	void (*free_op)(void *ptr);
};

struct memory_maps {
	struct map *head;
	struct map *tail;
	size_t num_mappings;
};

#define MAX_KMALLOC_SIZE 1024 * 1024 * 4

#ifndef __KERNEL__
#define ALLOC_MAX4M_REGULAR(x)		malloc(x)
#define FREE_MAX4M_REGULAR(x)		free(x)
#define ALLOC_MAX4M_DMA(x)			malloc(x)
#define FREE_MAX4M_DMA(x)			free(x)
#define ALLOC_ATOMIC(x)				malloc(x)
#define FREE_ATOMIC(x)				free(x)
#define ALLOC_CONTIG_PHY_PAGES(x,y)	malloc(x)
#define FREE_CONTIG_PHY_PAGES(x,y)	free(x)
#define ALLOC_UNLIMITED_VIRT(x)		malloc(x)
#define FREE_UNLIMITED_VIRT(x)		free(x)

#define SMALL_ALLOC_FPTR			malloc
#define SMALL_FREE_FPTR				free
#define LARGE_ALLOC_PTR				malloc
#define LARGE_FREE_FPTR				free

#define PROT_PAGE_X(x,n)			mprotect(x,n, PROT_READ | PROT_EXEC)
#define PROT_PAGE_NX(x,n)			mprotect(x,n, PROT_READ | PROT_WRITE)
#define PROT_PAGE_RO(x,n)			mprotect(x,n, PROT_READ)
#else
#define ALLOC_MAX4M_REGULAR(x)		kmalloc(x, GFP_KERNEL)
#define FREE_MAX4M_REGULAR(x)		kfree(x)
#define ALLOC_MAX4M_DMA(x)			kmalloc(x, GFP_DMA)
#define FREE_MAX4M_DMA(x)			kfree(x)
#define ALLOC_ATOMIC(x)				kmalloc(x, GFP_ATOMIC)
#define FREE_ATOMIC(x)
#define ALLOC_CONTIG_PHY_PAGES(x,y)	__get_free_pages(x,y)
#define FREE_CONTIG_PHY_PAGES(x,y)	__free_pages(x,y)
#define ALLOC_ZEROED_PAGE(x)		get_zeroed_page(x)
#define FREE_ZEROED_PAGE(x)			free_page(x)
#define ALLOC_UNLIMITED_VIRT(x)		vmalloc(x)
#define FREE_UNLIMITED_VIRT(x)		vfree(x)

#define SMALL_ALLOC_FPTR			kmalloc
#define SMALL_FREE_FPTR				kfree
#define LARGE_ALLOC_PTR				valloc
#define LARGE_FREE_FPTR				vfree

#define PROT_PAGE_X(x,n)			set_memory_x((unsigned long) x,n)
#define PROT_PAGE_NX(x,n)			set_memory_nx((unsigned long) x,n)
#define PROT_PAGE_RO(x,n)			set_memory_ro((unsigned long) x,n)
#endif /* __KERNEL__ */

#ifndef __KERNEL__
#define printk	printf
#define KERN_ALERT	""
#endif

#endif /* _SYSTEM_H */

