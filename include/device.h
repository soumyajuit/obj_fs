#ifndef _H_DEVICE_DOT_H
#define _H_DEVICE_DOT_H

class device {
    private:
        driver& device_driver;
        struc
    public:
        device();
        int init_device();
        int start_device();
        int stop_device();
        int uninit_device();
};


#endif
