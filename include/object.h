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
            unsigned char   obj_data[];
        } obj_internal;

    public:
        bool    obj_sanity_check();
        int     read_obj();
        int     write_obj();
        int     to_buffer();
        int     from_buffer();
}

#endif
