#ifndef STRUCTURES_H
#define STRUCTURES_H

// Node definitions
typedef struct node {
	char c;
	struct node * next;
	struct node * prev;
} Node;

// Buffer definition
typedef struct {
	Node * head;
	int size;
} Buffer;

// Node function declarations
Node * create_node(char c, Node *prev, Node *next);
void remove_node(Node *node);

// Buffer function declarations
Buffer * create_buffer(int size);
void delete_buffer(Buffer * buf);

#endif
