#include "cpp_hdr.h"
#include "c_hdr.h"
#include "common_hdr.h"
#include "object.h"

bool
object_c::obj_sanity_check()
{
    if (this->obj_internal.obj_magic != OBJ_MAGIC) {
        return false;
    }

    return true;
}

int
object_c::read_obj(const device dev)
{
    return 0;
}

int
object_c::write_obj(const device dev)
{
    return 0;
}

//Perform Deep Copy
int
object_c::to_buffer(char* buffer)
{
    return 0;
}

//Perform Deep Copy
int
object_c::from_buffer(const char* buffer)
{
    return 0;
}

const unsigned char*
object::get_obj_data()
{
    if (!this->obj_sanity_check()) {
        return NULL;
    }

    return this->obj_internal.obj_data;
}


