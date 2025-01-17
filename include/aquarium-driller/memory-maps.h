#ifndef _MEMORY_MAPS_H
#define _MEMORY_MAPS_H

int			setup_maps(void);
size_t		get_memory_maps_size(void);
struct map	*get_random_map(void);
struct map	* get_first_map(void);
struct map	* get_last_map(void);
void		destroy_maps(void);
void		destroy_buffers(void);
int			init_buffers(void);

#endif /* _MEMORY_MAPS_H */

