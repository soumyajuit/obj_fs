#include "cpp_hdr.h"
#include "c_hdr.h"
#include "common_hdr.h"



bool
object_c::obj_sanity_check()
{

    if (this->size != sizeof(this.obj_internal)) {
        return false;
    }

    if (this->obj_internal.obj_magic != OBJ_MAGIC) {
        return false;
    }

    return true;
}

int
object_c::read_obj()
{


}


int
object_c::write_obj()
{


}

//Perform Deep Copy
int
object_c::to_buffer()
{

}

//Perform Deep Copy
int
object_c::from_buffer()
{

}
