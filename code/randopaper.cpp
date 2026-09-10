#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <fileapi.h>

const int buffer_size = 255;
char read_buffer[buffer_size];

int check_file(char *file_path);
void set_file(char *file_path);
void get_file(void);

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 && check_file(argv[i + 1]))
        {
            set_file(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-g") == 0)
        {
            get_file();
        }
    }
}

int check_file(char *file_path)
{
    int result = 0;
    WIN32_FIND_DATA find_data;
    HANDLE file_handle = FindFirstFileA(file_path, &find_data);
    if (file_handle != INVALID_HANDLE_VALUE)
    {
        result = 1;
    }
    FindClose(file_handle);

    return result;
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
    printf("Wallpaper: %s", read_buffer);
}
