#ifndef STRUCTURES_H
#define STRUCTURES_H

// Node definitions
typedef struct node Node;

struct node {
	char c;
	Node * next;
	Node * prev;
};

// Line definition
typedef struct line Line;

struct line {
	Node * head;
	int length;
	Line * next;
	Line * prev;
};

// Node function declarations
Node * create_node(char c, Node *prev, Node *next);
void remove_node(Node *node);

// Line function declarations
Line * create_line(int size);
void delete_line(Line * line);
void delete_all_lines(Line * head);

#endif
