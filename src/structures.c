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
	if (node->prev != NULL)
		node->prev->next = node->next;
	
	if (node->next != NULL)
		node->next->prev = node->prev;

	// Free memory
	free(node);
}

// Remove a line
void remove_line(Line *line)
{
	// Break connections if they exist
	if (line->prev != NULL)
		line->prev->next = line->next;

	if (line->next != NULL)
		line->next->prev = line->prev;

	// Free memory
	delete_line(line);
}

// Create a Line 
Line * create_line(Line *prev, Line *next)
{
	// Allocate enough memory
	Line * new = malloc(sizeof(Line));
	new->length = 0;

	// Make a blank start node
	new->head = create_node(0, NULL, NULL);
	new->next = next;
	new->prev = prev;

	return new;
}

// Delete a Line 
void delete_line(Line * line)
{
	// Start at the head node
	Node * cur = line->head;
	Node * temp;

	while (cur != NULL)
	{
		// Store the next node
		temp = cur->next;

		// Free the current node
		free(cur);

		// Move to the next node
		cur = temp;
	}

	// Free the line itself
	free(line);
}

// Delete a list of lines
void delete_all_lines(Line * head)
{
	// Start at the first line
	Line * current = head;
	Line * temp;
	
	while (current != NULL)
	{
		// Store next line
		temp = current->next;

		// Delete all nodes in current line
		delete_line(current);

		// Move to the next line
		current = temp;
	}
}
