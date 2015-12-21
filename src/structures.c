#include <stdlib.h>
#include "structures.h"

// Create a char node
Node * create_node (char c, Node *prev, Node *next)
{
	Node * new = malloc(sizeof(Node));
	new->c = c;
	new->prev = prev;
	new->next = next;
	return new;
}

// Remove a char node
void remove_node (Node *node)
{
	// Break connection
	node->prev->next = node->next;
	
	if (node->next != NULL)
		node->next->prev = node->prev;

	// Free memory
	free(node);
}

// Create a buffer
Buffer * create_buffer (int size)
{
	Buffer * new = malloc(sizeof(Buffer));
	new->size = size;
	return new;
}

// Delete a buffer
void delete_buffer (Buffer * buf)
{
	// Start at the head node
	Node * cur = buf->head;
	Node * temp;
	while (cur != NULL)
	{
		temp = cur;
		cur = cur->next;
		free(temp);
	}
	free(buf);
}
