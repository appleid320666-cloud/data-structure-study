#include <stdio.h>

typedef struct Node {
    int value;
    struct Node *prev;
    struct Node *next;
} Node;

static Node *link_nodes(Node nodes[], size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i) {
        nodes[i].prev = i == 0 ? NULL : &nodes[i - 1];
        nodes[i].next = i + 1 == count ? NULL : &nodes[i + 1];
    }
    return count == 0 ? NULL : nodes;
}

/* Transfer the nodes of Lt1 into Lt2. Both lists are disjoint. */
static void merge_lists(Node **lt1, Node **lt2)
{
    Node *a = *lt1;
    Node *b = *lt2;
    Node sentinel = {0, NULL, NULL};
    Node *tail = &sentinel;

    while (a != NULL && b != NULL) {
        Node **source = a->value <= b->value ? &a : &b;
        Node *node = *source;
        *source = node->next;
        tail->next = node;
        node->prev = tail;
        tail = node;
    }
    tail->next = a != NULL ? a : b;
    if (tail->next != NULL) {
        tail->next->prev = tail;
    }
    *lt2 = sentinel.next;
    if (*lt2 != NULL) {
        (*lt2)->prev = NULL;
    }
    *lt1 = NULL;
}

int main(void)
{
    Node first[] = {{4, NULL, NULL}, {5, NULL, NULL},
                    {7, NULL, NULL}, {10, NULL, NULL}};
    Node second[] = {{2, NULL, NULL}, {5, NULL, NULL}, {8, NULL, NULL},
                     {12, NULL, NULL}, {13, NULL, NULL}};
    Node *lt1 = link_nodes(first, sizeof first / sizeof first[0]);
    Node *lt2 = link_nodes(second, sizeof second / sizeof second[0]);
    const Node *p;
    merge_lists(&lt1, &lt2);

    printf("Lt2 = {");
    for (p = lt2; p != NULL; p = p->next) {
        printf("%s%d", p == lt2 ? "" : ", ", p->value);
    }
    puts("}");
    return 0;
}
