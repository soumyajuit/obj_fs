#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

#define K       1024
#define M       (K * K)
#define G       (K * M)


const char* fsbackend_filename = "fs1_backend.data\0";

typedef unsigned int block_id_t;
typedef unsigned int fileid_t;

#define OBJ_SZ          (32 * K)
#define BITMAP_MAGIC    0xffbb
#define DATA_MAGIC      0xffdd
#define FILETAB_MAGIC   0xffaa
#define FS_OBJ_MAGIC    0xff11
#define FS_MAGIC        0xff33

#define BLOCK_SIZE   (1 * K)
#define MAX_FILE_SZ (100 * K)
#define MAX_NAME    20

typedef struct object_s {
    unsigned int    obj_magic;

    // If more than 1 type meta blocks
    // are embedded.
    unsigned int    obj_embedd_magic;
    unsigned int    obj_sz;
    unsigned int    obj_acc_sz;
    unsigned int    obj_free;
    unsigned char   obj_data[];
} object_t;

typedef struct filesys_s {
    char         fs_backend_file_name [MAX_NAME];
    unsigned int fs_magic;
    unsigned int fs_blk_sz;
    unsigned int fs_max_file_sz;
    unsigned int fs_min_file_sz;
    unsigned int fs_bitmap_size;
    unsigned int fs_file_tab_size;
    unsigned int fs_udata_sz;
    unsigned int fs_total_obj;
    unsigned int fs_total_files;

    unsigned long long fs_filetab_obj_loc;
    unsigned int       fs_filetab_obj_cnt;

    unsigned long long fs_bitmap_obj_loc;
    unsigned int       fs_bitmap_obj_cnt;

    unsigned long long fs_udata_obj_loc;
    unsigned int       fs_udata_obj_cnt;

    unsigned long long fs_size_in_bytes;
} filesys_t;

typedef struct filesys_incore_s {
    object_t*    fs_obj;
    filesys_t*   fs_disk;
    int          fs_fd;
} filesys_incore_t;

typedef struct udata_s {
    unsigned int udata_sz;
    unsigned char*  udata;
} udata_t;

typedef struct filetab_ent_s {
    fileid_t    file_id;
    unsigned int block_list[];
} filetab_ent_t;

typedef struct filetab_s {
    unsigned int ftab_fcount;
    unsigned int cur_free;
    unsigned int ftab_arr[];
} filetab_t;

typedef struct filename_tab_s {
    char            fname[MAX_NAME];
    fileid_t        fid;
} filename_tab_t;

typedef struct bitmap_s {
} bitmap_t;

typedef struct fincore_s {
    fileid_t        fid;
    unsigned int    foff;
} fincore_t;


static filesys_incore_t*   local_fs = NULL;

#define CURR_FS     local_fs

static inline unsigned int
fs_get_fblock_list_sz(filesys_t* fs)
{
    return ((fs->fs_max_file_sz / fs->fs_blk_sz) + 1) * sizeof(block_id_t);
}

static inline unsigned int
fs_get_max_file_no(filesys_t* fs)
{
    return ((fs->fs_udata_sz / fs->fs_min_file_sz)  + 1);
}


static int
fs_read_object(int fd, object_t* obj, unsigned int size )
{
    int ret = 0;
    if ((ret = read(fd, (void*) obj, size)) < 0) {
        printf("\n Read failed \n");
    }

    return ret;
}

static int
fs_write_object(int fd, object_t* obj, unsigned int size)
{
    int ret = 0;
    if ((ret = write(fd, (const void*) obj, size)) < 0) {
        printf("\n Write failed \n");
    }

    if ((ret = fsync(fd)) < 0) {
        printf("\n Sync failed \n");
    }

    return ret;
}

static unsigned int
get_object_size(unsigned int obj_type)
{
    switch(obj_type) {
        case BITMAP_MAGIC:
            return sizeof(bitmap_t);
        case DATA_MAGIC:
            return sizeof(udata_t);
        case FILETAB_MAGIC:
            return sizeof(filetab_t);
        default:
            return sizeof(object_t);
     }
}

static void
print_obj(object_t* obj)
{
    printf("\n %x \n", obj->obj_magic);
    printf("\n %u \n", obj->obj_acc_sz);
}

static int
fill_and_write_obj(int fd, object_t* obj)
{
    if (obj == NULL) {
        return -1;
    }

    obj->obj_magic =  DATA_MAGIC;
    obj->obj_acc_sz = sizeof(unsigned char) * 10;
    obj->obj_data[0] = 'S';
    obj->obj_data[1] = 'O';
    obj->obj_data[2] = 'U';
    obj->obj_data[3] = 'M';
    print_obj(obj);
    return fs_write_object(fd, obj, sizeof(object_t) + obj->obj_acc_sz);
}

static int
read_and_fill_obj(int fd, object_t* obj)
{
    int ret = 0;
    ret = fs_read_object(fd, obj, sizeof(object_t) + obj->obj_acc_sz);
    return ret;
}

static int
fs_check_fs(filesys_t* fs_disk)
{
    printf("\n %x \n", fs_disk->fs_magic);
    if (fs_disk == NULL) {
        return -1;
    }

    if (fs_disk->fs_magic != FS_MAGIC) {
        return -1;
    }
    //do other valid checks.

    return 0;
}

static int
fs_write_superblock(filesys_incore_t* fs_incore)
{
    object_t* fs_obj = fs_incore->fs_obj;
    fs_obj->obj_magic = FS_OBJ_MAGIC;
    fs_obj->obj_acc_sz = sizeof(filesys_t);
    fs_obj->obj_free_sz = OBJ_SZ - sizeof(object_t) + obj->obj_acc_sz;
    return fs_write_object(fs_incore->fs_fd, fs_obj, OBJ_SZ);
}

static int
fs_read_superblock(filesys_incore_t* fs_incore)
{
    object_t* fs_obj = fs_incore->fs_obj;
    fs_obj->obj_magic = FS_OBJ_MAGIC;
    fs_obj->obj_acc_sz = sizeof(filesys_t);
    return fs_read_object(fs_incore->fs_fd, fs_obj, OBJ_SZ);
}

static int
fs_create_or_mount_fs(unsigned long long size)
{
    int fd = 1;
    int ret = 0;
    int if_create = 0;

    object_t* fs_obj = (object_t*) malloc(OBJ_SZ);

    filesys_t* fs_disk = (filesys_t*) &fs_obj->obj_data[0];
    filesys_incore_t *fs = (filesys_incore_t*) malloc(sizeof(filesys_incore_t));

    fs->fs_disk = fs_disk;
    fs_disk->fs_size_in_bytes = size;

    if (access(fsbackend_filename, F_OK | R_OK | W_OK) < 0) {
        if ((fd = open(fsbackend_filename, O_CREAT | O_RDWR)) < 0) {
            printf("\n Cannot open file \n");
            exit(255);
        }
        if_create = 1;
    } else {
        if ((fd = open(fsbackend_filename, O_RDWR)) < 0) {
            printf("\n Cannot open file \n");
            exit(255);
        }
    }

    fs->fs_fd = fd;
    fs->fs_obj = fs_obj;

    if (if_create &&
        ((ret = ftruncate(fd, fs_disk->fs_size_in_bytes)) < 0) ){

        printf("\n backend file extension failed \n");
        goto err;
    }

    if (if_create) {
        strcpy(fs_disk->fs_backend_file_name, fsbackend_filename);
        fs_disk->fs_blk_sz = BLOCK_SIZE;
        fs_disk->fs_max_file_sz = MAX_FILE_SZ;
        fs_disk->fs_min_file_sz = BLOCK_SIZE;
        fs_disk->fs_udata_sz = fs->fs_disk->fs_size_in_bytes;
        fs_disk->fs_magic = FS_MAGIC;
        fs_disk->fs_total_obj = fs_disk->fs_size_in_bytes / OBJ_SZ;
    } else {
        fs_read_superblock(fs);
        if (fs_check_fs(fs_disk) < 0) {
            printf("\n The superblock is corrupted \n");
            goto err;
        }
    }

    if (if_create) {
        fs_write_superblock(fs);
    }

    CURR_FS = fs;

    goto out;

err:
    free(fs);
    if (fd < 0) {
        close(fd);
    }
out:
    return ret;
}

static int
fs_init(filesys_incore_t* fs)
{
    filetab_init(fs);
}

int
main(int argc, char* argv[])
{
    fs_create_or_mount_fs(1 * G);
    fs_init(CURR_FS);
    return 0;
}

