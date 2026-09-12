typedef enum FileType {
    FT_NONE = 0,
    FT_DIRECTORY = 1,
    FT_FILE = 2,
} file_type;

char *valid_extensions[] = {
    "jpg",  "jpeg", "bmp",  "dib",   "png",  "jfif",  "jpe",   "gif",
    "tif",  "tiff", "wdp",  "heic",  "heif", "heics", "heifs", "hif",
    "avci", "avcs", "avif", "avifs", "jxr",  "jxl",   "webp",
};

const int buffer_size = MAX_PATH;
char read_buffer[buffer_size];

int validate_extention(char *file_path);
file_type check_file(char *file_path);
void set_file(char *file_path);
void set_dir(char *file_path);
void get_file(void);
void list_files(char *dir_path);
