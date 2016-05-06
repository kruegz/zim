#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include "ascii.h"
#include "structures.h"

// Keep track of position and size of window
int r, c, nrows, ncols;

Line * read_file(char * filename, Line *line);
void write_file(char * filename, Line *line);
void shiftRestRight();
void drawLine(int row, Line *line);
void shiftLinesDown(int row, Line *start);
void skipWhitespace();
void draw(char dc, int *row, int *col, Line *currentLine);
void backspace();

int main(int argc, char ** argv)
{
	// Declare
	int d;

	// Line object
	Line *headLine= create_line(NULL, NULL); 
	Node *currentNode = headLine->head;	
	Line *currentLine = headLine;

	// File declarations
	char filename[100];
	bool fileLoaded = 0;

	// Check for arguments
	if (argc > 1)
	{
		// Get the filename	
		strcpy(filename, argv[1]);

		// Open the file and check for error
		headLine = read_file(filename, headLine);

		fileLoaded = 1;
	}

	// Setup window
	WINDOW *wnd;
	wnd = initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(wnd, nrows, ncols);
	clear();
	refresh();

	// Print characters if file was loaded
	if (fileLoaded)
	{
		// Loop through lines
		Line * tempLine = headLine;
		Node * tempNode;

		while (tempLine != NULL)
		{
			// Loop through characters in line
			tempNode = tempLine->head;

			while (tempNode != NULL)
			{
				// Do appropriate action with character
				if (tempNode->c != '\0')
				{
					// Draw character
					draw(tempNode->c, &r, &c, currentLine);
				}

				// Move to next character
				tempNode = tempNode->next;
				currentNode = tempNode;
			}

			// Move to next line
			if (tempLine->next != NULL && r < nrows)
			{
				r++;
				c = 0;
			}

			move(r, c);

			// Move down the linked list
			currentLine = tempLine;
			tempLine = tempLine->next;
		}

		// Set current node to end of loaded file
		/*currentNode = tempNode->prev->prev;*/
	}

	// Main input loop
	while ((d = getch()) != ESCAPE)
	{
		if (d == BACKSPACE || d == DELETE || d == KEY_BACKSPACE || d == KEY_DC)
		{
			// Check if we're on the first character of a line
			if (currentNode == NULL)
			{
				// Check if there is a line before this one
				if (currentLine->prev != NULL)
				{
					Line *temp = currentLine;
					/*Node *oldNode = currentNode;*/

					// Attach the remainder of the current line to the previous line
					/*currentNode->prev = currentLine->prev->tail;*/
					/*currentLine->prev->tail->next = currentNode;*/
					/*currentLine->prev->tail = currentLine->tail;*/

					currentLine->prev->next = currentLine->next;

					/*if (currentLine->next != NULL)*/
					/*currentLine->next->prev = currentLine->prev;*/

					// Move to the end of the previous line
					currentLine = currentLine->prev;
					currentNode = currentLine->tail;

					/*while (currentNode->next != NULL)*/
					/*currentNode = currentNode->next;*/

					// Delete the node
					/*remove_node(oldNode);*/

					// Delete the line
					free(temp);

				}
			}
			else 
			{
				// Remove the previous node
				Node *temp = currentNode;

				// Keep track of head node
				if (currentNode == currentLine->head)
					currentLine->head = currentNode->next;

				// Keep track of tail node
				if (currentNode == currentLine->tail)
					currentLine->tail = currentNode->prev;

				currentNode = currentNode->prev;

				remove_node(temp);
			}

			// Perform the visual backspace
			backspace();

		}
		else if (d == NEWLINE || d == KEY_ENTER)
		{
			// Insert new line
			currentLine->next = create_line(currentLine, NULL);
			currentLine = currentLine->next;
			currentNode = currentLine->head;

			// Move to beginning of next line
			if (r < nrows)
			{
				r++;
				c = 0;
			}
			move(r, c);
		}
		else
		{
			// Insert node
			if (currentNode != NULL)
			{
				// Insert a new node after the current
				currentNode->next = create_node(d, currentNode, currentNode->next);

				// Keep track of tail node
				if (currentNode == currentLine->tail)
					currentLine->tail = currentNode->next;

				currentNode = currentNode->next;
			}
			else
			{
				currentLine->head = create_node(d, NULL, NULL);
				currentNode = currentLine->head;
				currentLine->tail = currentNode;
			}

			/*shiftRestRight();*/
			draw(d, &r, &c, currentLine);
		}
	}

	// Close window
	endwin();

	// Ask if user wants to save
	printf("Would you like to save? (y/n): ");
	fflush(stdout);
	char save;
	scanf("%c", &save);

	if (save == 'y' || save == 'Y')
	{
		// Get the filename
		printf("Enter the filename to write: ");
		fflush(stdout);
		scanf("%s", filename);
		write_file(filename, headLine);
	}

	// Delete buffer
	delete_all_lines(headLine);

	return 0;
}

// Read data from file into buffer
Line * read_file(char * filename, Line * headLine)
{
	FILE *fp;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("Error opening file to read\n");
		fflush(stdout);
		exit(1);
	}

	int d;
	Line *currentLine = headLine;
	Node *currentNode = headLine->head;

	while ((d = fgetc(fp)) != EOF)
	{
		// Check for newline
		if (d == NEWLINE)
		{
			// Create a new line
			currentLine->next = create_line(currentLine, NULL);
			currentLine = currentLine->next;
			currentNode = currentLine->head;
		}
		else
		{
			if (currentNode != NULL)
			{
				// Insert a new node after the current
				currentNode->next = create_node(d, currentNode, currentNode->next);

				// Keep track of tail node
				if (currentNode == currentLine->tail)
					currentLine->tail = currentNode->next;

				currentNode = currentNode->next;
			}
			else
			{
				currentLine->head = create_node(d, NULL, NULL);
				currentNode = currentLine->head;
				currentLine->tail = currentNode;
			}
		}
	}

	fclose(fp);

	return headLine;
}

// Write buffer to file
void write_file(char * filename, Line * head)
{
	FILE *fp;

	// Open file
	if((fp = fopen(filename, "w+")) == NULL)
	{
		printf("Error occurred open file to save");
		exit(1);
	}

	// Write each line
	Line *currentLine = head;

	while (currentLine != NULL)
	{
		// Write the characters in the line
		Node *currentNode = currentLine->head;

		while (currentNode != NULL)
		{
			// Write character to file
			fputc(currentNode->c, fp);

			// Go to next character
			currentNode = currentNode->next;
		}

		// Write a newline to file
		fputc('\n', fp);

		// Go to next line
		currentLine = currentLine->next;
	}

	// Close file
	fclose(fp);
}

void shiftRestRight()
{
	int temp = c;
	c = ncols;
	while (c > temp)
	{
		mvdelch(r, c);
		mvinsch(r, c, mvinch(r, c-1));
		refresh();
		c--;
	}
	c = temp;
	move(r, c);
}

// Draw a character at position
void draw (char dc, int *r, int *c, Line *currentLine)
{
	// Move to position
	move(*r, *c);

	// Replace the character there
	delch();
	insch(dc);

	// Refresh the window
	refresh();

	// Check if there is room on the currentline
	if (*c < ncols-1)
	{
		// Advance column
		if (*c < ncols)
			(*c)++;
	}
	else
	{
		// Shift following rows down on display
		shiftLinesDown(*r+1, currentLine->next);			

		// Move to next line
		if (*r < nrows)
		{
			(*r)++;
			*c = 0;
		}
	}
	
	// Move to new position
	move(*r, *c);
}

// Move back to non-whitespace
void skipWhitespace()
{
	// Move to the end of the previous row
	c = ncols;
	r--;
	move(r, c);	

	// Move left until not whitespace
	while ((inch() & A_CHARTEXT) == 32 && c > 0)
	{
		c--;
		move(r, c);
	}
	if ((inch() & A_CHARTEXT) != 32)
	{
		if (c == 0)
			c++;
		else
			c++;
	}
	move(r, c);
	refresh();
}

// Draw the characters in a line on a row
void drawLine(int row, Line *line)
{
	int tempRow = row, tempCol = 0;
	Node *currentNode = line->head;

	move(tempRow, tempCol);

	while (currentNode != NULL)
	{
		// Replace the character there
		delch();
		insch(currentNode->c);

		// Refresh the window
		refresh();

		// Advance column
		if (tempCol < ncols)
			tempCol++;

		// Move to the current position
		move(tempRow, tempCol); 

		currentNode = currentNode->next;
	}

	refresh();
}

// Shift all lines from row to the end of the screen down one line
void shiftLinesDown(int row, Line *start)
{
	int currentRow = row;
	Line *currentLine = start;

	// Loop while there is room and lines to draw
	while (currentRow < nrows && currentLine != NULL)
	{
		currentRow++;
		drawLine(currentRow, currentLine);
		currentLine = currentLine->next;
	}
}

// Perform a backspace
void backspace()
{
	// Check to see if we're deleting a newline
	if (c == 0)
	{
		if (r != 0)
			skipWhitespace();
	}
	else
	{	
		// Move a space back
		if (c > 0)
			c--;

		// Move to the current position
		move(r, c);

		// Delete the character there
		delch();

		// Refresh the window
		refresh();
	}
}
