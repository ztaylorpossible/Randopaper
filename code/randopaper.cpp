#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <fileapi.h>

const int buffer_size = MAX_PATH;
char read_buffer[buffer_size];

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
    printf("Wallpaper: %s\n", read_buffer);
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
        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            printf("File: %s\n", find_data.cFileName);
        }
    } while (FindNextFile(file_handle, &find_data));
    FindClose(file_handle);
}
