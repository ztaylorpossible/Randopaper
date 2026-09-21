#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <fileapi.h>
#include <processenv.h>

#include "randopaper.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc < 2)
    {
        select_random();
        return 0;
    }

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
        else if (strcmp(argv[i], "-o") == 0)
        {
            open_appdata();
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            list_dirs();
            i++;
        }
        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc)
        {
            add_dir(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc)
        {
            remove_dir(argv[i + 1]);
            i++;
        }
    }
}

void select_random(void)
{
    HANDLE file_handle;
    char *env_filepath = "%appdata%\\Randopaper\\directories.txt";
    char filepath[MAX_PATH];
    ExpandEnvironmentStringsA(env_filepath, filepath, MAX_PATH);
    file_handle = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        CloseHandle(file_handle);
        printf("Failed to open file.\n");
        return;
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(file_handle, &file_size);
    char *file_read_buffer = (char *)malloc(file_size.QuadPart);

    if (file_read_buffer == NULL)
    {
        printf("Failed to allocate file read buffer.\n");
        CloseHandle(file_handle);
        free(file_read_buffer);
        return;
    }
    *file_read_buffer = NULL;

    DWORD bytes_read;
    ReadFile(file_handle, file_read_buffer, file_size.QuadPart - 1, &bytes_read, NULL);

    string_list_t *directory_list = string_list_new();
    char *current = file_read_buffer;
    int counter = 0;
    while(*(current + counter) != '\0')
    {
        counter++;
        if (*(current + counter) == '\n' || *(current + counter) == '\0')
        {
            char directory[MAX_PATH];
            strncpy(directory, current, counter);
            string_list_add(directory_list, directory);
            if (*(current + counter) == '\n')
            {
                current++;
            }
            current += counter;
            counter = 0;
        }
    }

    string_list_t *file_list = string_list_new();
    for (int i = 0; i < directory_list->count; i++)
    {
        char *directory = string_list_get_index(directory_list, i);
        char *dir_search_path = (char *)malloc(strlen(directory) + 3);
        strcpy(dir_search_path, directory);
        strcat(dir_search_path, "\\*");

        WIN32_FIND_DATA find_data;
        file_handle = FindFirstFileA(dir_search_path, &find_data);
        if (file_handle == INVALID_HANDLE_VALUE)
        {
            printf("Directory %s is empty.\n", dir_search_path);
            FindClose(file_handle);
            free(dir_search_path);
            continue;
        }

        do
        {
            if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0
                && validate_extension(find_data.cFileName))
            {
                char *full_path = (char *)malloc(strlen(directory) + strlen(find_data.cFileName) + 2);
                strcpy(full_path, directory);
                strcat(full_path, "\\");
                strcat(full_path, find_data.cFileName);
                string_list_add(file_list, full_path);
                free(full_path);
            }
        } while (FindNextFile(file_handle, &find_data));

        free(dir_search_path);
    }

    int selection = rand() % file_list->count;
    char *file_path = string_list_get_index(file_list, selection);
    SystemParametersInfoA(SPI_SETDESKWALLPAPER,
                          0,
                          file_path,
                          SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);

    string_list_free(directory_list);
    string_list_free(file_list);
    CloseHandle(file_handle);
    free(file_read_buffer);
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

void list_dirs()
{
    string_list_t *directory_list = get_dir_string_list();
    if (directory_list == NULL)
    {
        return;
    }

    for (int i = 0; i < directory_list->count; i++)
    {
        printf("%d: %s\n", i, string_list_get_index(directory_list, i));
    }
    string_list_free(directory_list);
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

void remove_dir(char *index_string)
{
    if (index_string == NULL)
    {
        return;
    }

    string_list_t *directory_list = get_dir_string_list();
    if (directory_list == NULL)
    {
        return;
    }
    int index = atoi(index_string);
    string_list_remove(directory_list, index);

    HANDLE file_handle;
    char env_filepath[] = "%appdata%\\Randopaper\\directories.txt";
    char filepath[MAX_PATH];
    ExpandEnvironmentStringsA(env_filepath, filepath, MAX_PATH);
    DeleteFileA(filepath);
    if (directory_list->count <= 0)
    {
        free(directory_list);
        return;
    }

    file_handle = CreateFileA(filepath, FILE_APPEND_DATA, FILE_SHARE_READ,
                              NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        CloseHandle(file_handle);
        free(directory_list);
        printf("Failed to open or create file.\n");
        return;
    }

    for (int i = 0; i < directory_list->count; i++)
    {
        DWORD bytes_written;
        char *dir = string_list_get_index(directory_list, i);
        char *write_dir = (char *)malloc(strlen(dir) + 2);
        if (write_dir == NULL)
        {
            printf("Failed memory allocation for writing directory.\n");
            CloseHandle(file_handle);
            free(directory_list);
            return;
        }

        strcpy(write_dir, dir);
        strcat(write_dir, "\n");

        WriteFile(file_handle, write_dir, strlen(write_dir), &bytes_written, NULL);
        free(write_dir);
    }

    CloseHandle(file_handle);
    free(directory_list);
}

string_list_t *get_dir_string_list()
{
    HANDLE file_handle;
    char env_filepath[] = "%appdata%\\Randopaper\\directories.txt";
    char filepath[MAX_PATH];
    ExpandEnvironmentStringsA(env_filepath, filepath, MAX_PATH);
    file_handle = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        CloseHandle(file_handle);
        printf("Failed to retrieve directories");
        return NULL;
    }

    LARGE_INTEGER file_size;
    GetFileSizeEx(file_handle, &file_size);
    char *file_read_buffer = (char *)malloc(file_size.QuadPart);

    if (file_read_buffer == NULL)
    {
        printf("Failed to allocate file read buffer.\n");
        CloseHandle(file_handle);
        free(file_read_buffer);
        return NULL;
    }
    *file_read_buffer = NULL;

    DWORD bytes_read;
    ReadFile(file_handle, file_read_buffer, file_size.QuadPart - 1, &bytes_read, NULL);

    string_list_t *directory_list = string_list_new();
    if (directory_list == NULL)
    {
        printf("Failed to initialize string list for directories.\n");
        CloseHandle(file_handle);
        free(file_read_buffer);
        return NULL;
    }

    char *current = file_read_buffer;
    int counter = 0;
    while(*(current + counter) != '\0')
    {
        counter++;
        if (*(current + counter) == '\n' || *(current + counter) == '\0')
        {
            char directory[MAX_PATH];
            strncpy(directory, current, counter);
            directory[counter] = '\0';
            string_list_add(directory_list, directory);
            if (*(current + counter) == '\n')
            {
                current++;
            }
            current += counter;
            counter = 0;
        }
    }

    free(file_read_buffer);
    CloseHandle(file_handle);

    return directory_list;
}

void open_appdata(void)
{
    HANDLE file_handle;
    char env_filepath[] = "%appdata%\\Randopaper";
    char filepath[MAX_PATH];
    ExpandEnvironmentStringsA(env_filepath, filepath, MAX_PATH);
    ShellExecuteA(NULL, "explore", filepath, NULL, NULL, SW_SHOWNORMAL);
}
