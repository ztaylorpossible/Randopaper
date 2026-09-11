#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <fileapi.h>

char *valid_extensions[] = {
    "jpg",  "jpeg", "bmp",  "dib",   "png",  "jfif",  "jpe",   "gif",
    "tif",  "tiff", "wdp",  "heic",  "heif", "heics", "heifs", "hif",
    "avci", "avcs", "avif", "avifs", "jxr",  "jxl",   "webp",
};

const int buffer_size = MAX_PATH;
char read_buffer[buffer_size];

int validate_extention(char *file_path);
int check_file(char *file_path);
void set_file(char *file_path);
void get_file(void);
void list_files(char *dir_path);

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc && check_file(argv[i + 1]))
        {
            set_file(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-g") == 0)
        {
            get_file();
        }
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc)
        {
            list_files(argv[i + 1]);
            i++;
        }
    }
}

int validate_extension(char *file_path)
{
    int guard = 0;
    char *current = file_path;
    char *extension = NULL;

    while (*current != '\0' && guard < 1000)
    {
        if (*current == '.')
        {
            extension = current;
        }
        current++;
        guard++;
    }

    if (extension == NULL)
    {
        return 0;
    }
    extension++;

    size_t check_len = sizeof(valid_extensions) / sizeof(char *);
    for (int i = 0; i < check_len; i++)
    {
        if (strcmp(extension, valid_extensions[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int check_file(char *file_path)
{
    WIN32_FIND_DATA find_data;
    HANDLE file_handle = FindFirstFileA(file_path, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        FindClose(file_handle);
        return 0;
    }

    if (validate_extension(file_path))
    {
        FindClose(file_handle);
        return 1;
    }

    FindClose(file_handle);
    return 0;
}

void set_file(char *file_path)
{
    printf("Set\n");
    SystemParametersInfoA(SPI_SETDESKWALLPAPER,
                          0,
                          file_path,
                          SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);
}

void get_file()
{
    printf("Get\n");
    SystemParametersInfoA(SPI_GETDESKWALLPAPER, buffer_size, read_buffer, 0);
    printf("Wallpaper: %s\n", read_buffer);
    validate_extension(read_buffer);
}

void list_files(char *dir_path)
{
    WIN32_FIND_DATA find_data;
    HANDLE file_handle = FindFirstFileA(dir_path, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        printf("Invalid path\n");
        return;
    }

    if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        printf("Path is not a directory\n");
        return;
    }

    strcat(dir_path, "\\*");
    file_handle = FindFirstFileA(dir_path, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        printf("Directory is empty.\n");
        return;
    }

    printf("Getting files...\n");
    do
    {
        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 && validate_extension(find_data.cFileName))
        {
            printf("File: %s\n", find_data.cFileName);
        }
    } while (FindNextFile(file_handle, &find_data));
    FindClose(file_handle);
}
