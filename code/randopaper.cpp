#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <fileapi.h>
#include <processenv.h>

#include "randopaper.h"
#include "string_list.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc)
        {
            file_type ft = check_file(argv[i + 1]);
            if (ft == FT_DIRECTORY)
            {
                set_dir(argv[i + 1]);
            }
            else if (ft == FT_FILE)
            {
                set_file(argv[i + 1]);
            }
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
        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
        {
            add_dir(argv[i + 1]);
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

file_type check_file(char *file_path)
{
    WIN32_FIND_DATA find_data;
    HANDLE file_handle = FindFirstFileA(file_path, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        FindClose(file_handle);
        return FT_NONE;
    }

    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        FindClose(file_handle);
        return FT_DIRECTORY;
    }

    if (validate_extension(file_path))
    {
        FindClose(file_handle);
        return FT_FILE;
    }

    FindClose(file_handle);
    return FT_NONE;
}

void set_file(char *file_path)
{
    printf("Set\n");
    SystemParametersInfoA(SPI_SETDESKWALLPAPER,
                          0,
                          file_path,
                          SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);
}

void set_dir(char *input_path)
{
    char *search_path = (char *)malloc(strlen(input_path) + 3);
    char *dir_path = (char *)malloc(strlen(input_path) + 2);
    if (search_path == NULL)
    {
        printf("Failed to allocate search path\n");
        return;
    }
    if (dir_path == NULL)
    {
        printf("Failed to allocate dir path\n");
        free(search_path);
        return;
    }

    strcpy(search_path, input_path);
    strcat(search_path, "\\*");
    strcpy(dir_path, input_path);
    strcat(dir_path, "\\");

    WIN32_FIND_DATA find_data;
    HANDLE file_handle = FindFirstFileA(search_path, &find_data);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        printf("Directory is empty.\n");
        free(search_path);
        free(dir_path);
        FindClose(file_handle);
        return;
    }

    string_list_t *files = string_list_new();
    do
    {
        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 && validate_extension(find_data.cFileName))
        {
            char *full_path = (char *)malloc(strlen(dir_path) + strlen(find_data.cFileName) + 1);
            if (full_path == NULL)
            {
                continue;
            }
            strcpy(full_path, dir_path);
            strcat(full_path, find_data.cFileName);
            string_list_add(files, full_path);
        }
    } while (FindNextFile(file_handle, &find_data));

    int selection = rand() % files->count;
    char *file_path = string_list_get_index(files, selection);
    SystemParametersInfoA(SPI_SETDESKWALLPAPER,
                          0,
                          file_path,
                          SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);

    free(search_path);
    free(dir_path);
    string_list_free(files);
    FindClose(file_handle);
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
        FindClose(file_handle);
        return;
    }

    if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
        printf("Path is not a directory\n");
        FindClose(file_handle);
        return;
    }

    strcat(dir_path, "\\*");
    file_handle = FindFirstFileA(dir_path, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        printf("Directory is empty.\n");
        FindClose(file_handle);
        return;
    }

    printf("Getting files...\n");
    do
    {
        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            printf("File: %s\n", find_data.cFileName);
        }
        else
        {
            printf("Directory: %s\n", find_data.cFileName);
        }
    } while (FindNextFile(file_handle, &find_data));
    FindClose(file_handle);
}

void add_dir(char *dir_path)
{
    WIN32_FIND_DATA find_data;
    HANDLE file_handle = FindFirstFileA(dir_path, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE || ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0))
    {
        printf("Invalid directory\n");
        FindClose(file_handle);
        return;
    }
    FindClose(file_handle);

    char *env_appdata = "%appdata%";
    char appdata[MAX_PATH];
    ExpandEnvironmentStringsA(env_appdata, appdata, MAX_PATH);
    char *randopaper_dir = "\\Randopaper";
    if (strlen(appdata) + strlen(randopaper_dir) + 1 > MAX_PATH)
    {
        printf("AppData path too long\n");
        FindClose(file_handle);
        return;
    }
    strcat(appdata, randopaper_dir);

    file_handle = FindFirstFileA(appdata, &find_data);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        int result = CreateDirectoryA(appdata, NULL);
    }
    FindClose(file_handle);

    char *env_filepath = "%appdata%\\Randopaper\\directories.txt";
    char filepath[MAX_PATH];
    ExpandEnvironmentStringsA(env_filepath, filepath, MAX_PATH);
    file_handle = CreateFileA(filepath, FILE_APPEND_DATA, FILE_SHARE_READ,
                              NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        CloseHandle(file_handle);
        printf("Failed to open or create file.\n");
        return;
    }

    char *write_dir = (char *)malloc(strlen(dir_path) + 2);
    if (write_dir == NULL)
    {
        printf("Failed memory allocation for writing directory.\n");
        CloseHandle(file_handle);
        return;
    }

    strcpy(write_dir, dir_path);
    strcat(write_dir, "\n");

    DWORD bytes_written;
    WriteFile(file_handle, write_dir, strlen(write_dir), &bytes_written, NULL);

    CloseHandle(file_handle);
    free(write_dir);
}
