#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_list.h"

string_list_t *string_list_new(void)
{
    string_list_t *new_list = (string_list_t *)malloc(sizeof(string_list_t));
    if (new_list == NULL)
    {
        return NULL;
    }

    new_list->head = NULL;
    new_list->count = 0;
    return new_list;
}

void string_list_add(string_list_t *list, char *new_string)
{
    if (list == NULL || new_string == NULL)
    {
        return;
    }

    string_list_node_t *new_node = (string_list_node_t *)malloc(sizeof(string_list_node_t));
    if (new_node == NULL)
    {
        return;
    }
    new_node->data = (char *)malloc(sizeof(char *) * strlen(new_string) + 1);
    if (new_node->data == NULL)
    {
        free(new_node);
        return;
    }

    strcpy(new_node->data, new_string);
    new_node->prev = NULL;
    new_node->next = NULL;

    if (list->count == 0)
    {
        list->head = new_node;
    }
    else
    {
        string_list_node_t *end = list->head;
        while (end->next != NULL)
        {
            end = end->next;
        }
        end->next = new_node;
        new_node->prev = end;
    }

    list->count++;
}

void string_list_free(string_list_t *list)
{
    if (list == NULL)
    {
        return;
    }

    if (list->head == NULL)
    {
        list->count = 0;
        free(list);
        return;
    }

    if (list->head->next == NULL)
    {
        free(list->head->data);
        list->head->data = NULL;
        free(list->head);
        list->head = NULL;
        list->count = 0;
        free(list);
        return;
    }

    string_list_node_t *current = list->head;
    while(current->next->next != NULL)
    {
        current = current->next;
    }
    do
    {
        free(current->next->data);
        current->next->data = NULL;
        current->next->next = NULL;
        current->next->prev = NULL;
        free(current->next);

        if (current->prev != NULL)
        {
            current = current->prev;
        }
    } while (current->prev != NULL);
    free(current->data);
    current->data = NULL;
    current->next = NULL;
    free(current);
    list->head = NULL;
    list->count = 0;
    free(list);
}

int string_list_contains(string_list_t *list, char *query)
{
    if (list == NULL || query == NULL || list->head == NULL)
    {
        return 0;
    }

    string_list_node_t *current = list->head;
    while (current != NULL)
    {
        if (strcmp(current->data, query) == 0)
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void string_list_display(string_list_t *list)
{
    if (list == NULL || list->head == NULL)
    {
        return;
    }

    printf("Elements: %d\n", list->count);
    string_list_node_t *current = list->head;
    while (current != NULL)
    {
        printf("%s\n", current->data);
        current = current->next;
    }
}
