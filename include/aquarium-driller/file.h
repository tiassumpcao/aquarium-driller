#ifndef _FILE_H
#define _FILE_H

#define AD_CONTEXT_FILE	"/home/tiago/ad_context"

struct file* file_open(const char* path, int flags, int rights);
void file_close(struct file* file);
int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size);
int file_sync(struct file* file);

#define AD_SAVE_CONTEX_TO_DISK(ctx)									\
	({																\
	 	struct file *filp;											\
	 	filp = file_open(AD_CONTEXT_FILE, O_CREAT|O_TRUNC, 0666);	\
	 	file_write(filp, 0, ctx, strlen(ctx));						\
	 	file_sync(filp);											\
	 	file_close(filp);											\
	 })

#endif /* _FILE_H */

