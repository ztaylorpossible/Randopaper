#include <stdio.h>
#include <string.h>
#include <windows.h>

const int buffer_size = 255;
char read_buffer[buffer_size];

int main(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-s") == 0)
        {
            printf("Set\n");
            char *file_path = argv[i + 1];
            SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, file_path, SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);
        }
        else if (strcmp(argv[i], "-g") == 0)
        {
            printf("Get\n");
            SystemParametersInfoA(SPI_GETDESKWALLPAPER, buffer_size, read_buffer, 0);
            printf("Wallpaper: %s", read_buffer);
        }
    }
}
