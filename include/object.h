#ifndef OBJECT_H
#define OBJECT_H

#define OBJ_SZ          (32 * K)
#define OBJ_MAGIC       0x0f0f

class object_c {
    private:
        struct {
            unsigned int    obj_magic;

            // If more than 1 type meta blocks
            // are embedded.
            unsigned int    obj_embedd_magic;
            unsigned int    obj_sz;
            unsigned int    obj_acc_sz;
            unsigned int    obj_free;
            unsigned char*  obj_data;
        } obj_internal;

    public:
        const unsigned char*  get_obj_data();
        int     read_obj(const device dev);
        int     write_obj(const device dev);
        bool    obj_sanity_check();
        int     to_buffer(char* buffer);
        int     from_buffer(const char* buffer);
};

typedef object_c object;

#endif
