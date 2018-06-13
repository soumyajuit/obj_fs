#ifndef _FS_DEFINE_H
#define _FS_DEFINE_H

typedef unsigned int block_id_t;
typedef unsigned int fileid_t;

#define BITMAP_MAGIC    0xffbb
#define DATA_MAGIC      0xffdd
#define FILETAB_MAGIC   0xffaa
#define FS_OBJ_MAGIC    0xff11
#define FS_MAGIC        0xff33

#define BLOCK_SIZE   (1 * K)
#define MAX_FILE_SZ (100 * K)
#define MAX_NAME    20

#endif
