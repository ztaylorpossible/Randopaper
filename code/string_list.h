#pragma once

typedef struct StringListNode string_list_node_t;

typedef struct StringListNode
{
    char *data;
    string_list_node_t *prev;
    string_list_node_t *next;
} string_list_node_t;

typedef struct StringList
{
    string_list_node_t *head;
    int count;
} string_list_t;

string_list_t *string_list_new(void);
void string_list_add(string_list_t *list, char *new_string);
void string_list_remove(string_list_t *list, int index);
void string_list_free(string_list_t *list);
int string_list_contains(string_list_t *list, char *query);
void string_list_display(string_list_t *list);
char *string_list_get_index(string_list_t *list, int index);
