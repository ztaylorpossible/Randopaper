# Randopaper
# About
This is a simple command-line utility for randomizing a desktop wallpaper in Windows. It allows drawing from multiple directories, which improves over the standard Windows system. This allows you to have your desired backgrounds organized however you like.

It has only been tested on 64 bit Windows 11, but will likely work on older versions as well.
# Getting Started
Source should be compiled with your C compiler of choice. I chose to use `cl` which is packaged as a tool with Visual Studio. If you'd like to use this method, you can check out the Visual Studio help for details.

Using `cl`, you can build with a command like:
```
cl randopaper.cpp string_list.cpp user32.lib shell32.lib
```

Whichever compiler you use, make sure it is able to link to both the user32.lib and shell32.lib libraries, and you should be good to go.
# Usage
- To add directories for Randopaper to pull images from, run the command:
```
randopaper -a absolute_path_to_directory
```
Note: directories should be added without the terminating `/`
- To see what directories are currently being used, you can run:
```
randopaper -l
```
That will display an indexed list of all directories currently being used. To remove one, run:
```
randopaper -r index_to_directory_to_remove
```
- After at least one directory has been added, you can just run
```
randopaper
```
to run the program and set a random desktop image from your provided directories.

If you like, you can create a shortcut to the executable or a .bat file that runs it, and place it in your Windows startup folder, and it will then automatically run every time you log in so that each time you are greeted with a new image.
# Misc Other Commands
Here are some other commands you can run if you like:

- Print the path to the current wallpaper:
```
randopaper -g
```
- Set the wallpaper to a specific image:
```
randopaper -s path_to_image
```
- Set the wallpaper to a random image from a specific directory:
```
randopaper -s path_to_directory
```
- Open Windows Explorer at the appdata location where randopaper data is stored:
```
randopaper -o
```
