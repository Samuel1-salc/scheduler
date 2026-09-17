#include "fila.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

CircularList* CircularList_creat()
{
    CircularList* list = (CircularList*)malloc(sizeof(CircularList));
    list->begin = NULL;
    list->size = 0;
    return list;
}

Node* creatNode(PCB *processo){
    Node* node = (Node*)malloc(sizeof(Node));
    node->processo = processo;
    node->next = NULL;
    return node;

}

void CircularList_Insert(CircularList* list, PCB  *processo)
{
    Node* node = creatNode(processo);
    

    if (list->begin == NULL)
    {
        list->begin = node;
        node->next = list->begin;
    }
    else
    {
        Node* temp = list->begin;
        while (temp->next != list->begin)
        {
            temp = temp->next;
        }
        temp->next = node;
        node->next = list->begin;
    }
    list->size++;
}

void CircularList_Print(CircularList *list)
{
    if (list == NULL || list->begin == NULL)
    {
        printf("Lista vazia.\n");
        return;
    }

    Node *temp = list->begin;

    do
    {
        printf(
            "PID: %d | Prioridade: %d | Tempo restante: %d\n",
            temp->processo->pid,
            temp->processo->prioridade,
            temp->processo->tempo_restante
        );

        temp = temp->next;

    } while (temp != list->begin);
}

bool CircularList_Empty(CircularList* list){
    return list == NULL || list->begin == NULL;
}

void CircularList_Remove(CircularList *list, PCB *processo)
{
    if (list == NULL || list->begin == NULL)
    {
        return;
    }

    Node *temp = list->begin;
    Node *prev = NULL;

    do
    {
        if (temp->processo == processo)
        {
            /*
             * Caso 1:
             * A lista possui apenas um elemento.
             */
            if (temp->next == temp)
            {
                list->begin = NULL;
            }

            /*
             * Caso 2:
             * O elemento a ser removido é o primeiro.
             */
            else if (temp == list->begin)
            {
                Node *aux = list->begin;

                while (aux->next != list->begin)
                {
                    aux = aux->next;
                }

                aux->next = list->begin->next;
                list->begin = list->begin->next;
            }

            /*
             * Caso 3:
             * O elemento está no meio ou no final.
             */
            else
            {
                prev->next = temp->next;
            }

            free(temp);
            list->size--;

            return;
        }

        prev = temp;
        temp = temp->next;

    } while (temp != list->begin);
}
