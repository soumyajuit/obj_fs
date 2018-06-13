import sys;

GB1 = 1073741824
max_filename = 20
error = -1
MB1 = 1048576
KB1 = 1024
fs_filename = "test_filesys"
fs_filetable = "test_fs_ftable"

class fs_file:

    def __init__(self, name, size):
        self.name = name;
        self.size = size;
        self.file_blocks = list();

    def del_file(self):
        return self.file_blocks();

class filesystem:

    def __init__(self, name, blk_sz):
        self.fs_name = name;
        self.blk_sz  = blk_sz * 1024;
        self.fs_main_file =  self.fs_name + '.data';
        self.data = self.fs_name + 'udata' + '.data';
        self.bitmap_f = self.fs_name + 'bitmap' + '.data';
        self.fs_ftable_sz = self.calc_ftbl_sz();
        self.create_file_table();
        self.create_bitmaps();
        self.create_backend_file();

        self.file_count = 0;
        self.fs_max_size = 100 * KB1;

    def calc_ftbl_sz(self):
        return 450*MB1;

    def create_file_table(self):
        self.fs_ftable_name = self.fs_name + 'fstable'; 
        print "Creating fs_filetable ", self.fs_ftable_name
        self.fs_ftable = open(self.fs_ftable_name, 'w+');
        self.fs_ftable.truncate();

    def create_backend_file(self ):
        self.backend_f = open(self.data, 'w+');
        self.backend_f.truncate(2147483648);

    def create_bitmaps(self):
        self.bitmap_f = open(self.bitmap_f, 'w+');
        self.bitmap_f.truncate(20 * MB1);

    def create_fs_tbl_inmem(self):
        self.file_dict = dict();

    def create_file(self, filename, size):
        if self.file_count > self.file_count_max:
            return -1;
        if size > self.max_fs_size:
            return -1
        self.file_dict['filename'] = fs_file(filename, size)
        self.file_count += 1;
        return 0;

    def write_into_file(self, filename, offset, data_str):
        self.get_file_block();
        
#def read_fs_table(self):
    def write_fs_table(self):

#    def read_fs_bitmap(self):
    def write_fs_bitmap(self):

#    def read_udata_from(self):
    def write_udata_to(self):


if __name__ == '__main__':
    fs1 = filesystem('fs1', 4);
