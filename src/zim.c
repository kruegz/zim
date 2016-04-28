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
void skipWhitespace();
void draw (char dc);
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
					draw(tempNode->c);

				// Move to next character
				currentNode = tempNode;
				tempNode = tempNode->next;
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
			// Don't backspace first character
			if (currentNode != headLine->head)
			{
				// Check if we're on the first character of a line
				if (currentNode->prev == NULL)
				{
					// Check if there is a line before this one
					if (currentLine->prev != NULL)
					{
						Line *temp = currentLine;

						// Move to the end of the previous line
						currentLine = currentLine->prev;
						currentNode = currentLine->head;

						while (currentNode->next != NULL)
							currentNode = currentNode->next;

						// Delete the line
						remove_line(temp);
					}
				}
				else
				{
					// Remove the previous node
					Node *temp = currentNode;
					currentNode = currentNode->prev;

					remove_node(temp);
				}
				
				// Perform the visual backspace
				backspace();
			}
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
			currentNode->next = create_node(d, currentNode, currentNode->next);
			currentNode = currentNode->next;
			/*shiftRestRight();*/
			draw(d);
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
			currentNode->next = create_node(d, currentNode, NULL);	
			currentNode = currentNode->next;	
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
void draw (char dc)
{
	// Replace the character there
	delch();
	insch(dc);

	// Refresh the window
	refresh();

	// Advance column
	if (c < ncols)
		c++;

	// Move to the current position
	move(r, c);
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
