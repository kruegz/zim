#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include "ascii.h"
#include "structures.h"

// Keep track of position and size of window
int r, c, nrows, ncols;
Node * current;

Node * read_file(char * filename, Buffer * buf);
void write_file(char * filename, Buffer *buf);
void left();
void right();
void up();
void down();
void shiftRestRight();
void skipWhitespace();
void draw (char dc);
void backspace();

int main(int argc, char ** argv)
{
	// Declare
	int d;

	// Buffer object
	Buffer *buf = create_buffer(1024); 
	buf->head = create_node(0, NULL, NULL);
	current = buf->head;	

	// File declarations
	FILE *fp;
	char filename[100];
	bool fileLoaded = 0;

	// Check for arguments
	if (argc > 1)
	{
		fileLoaded = 1;

		// Get the filename	
		strcpy(filename, argv[1]);

		// Open the file and check for error
		current = read_file(filename, buf);


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
		Node * temp = buf->head->next;
		while (temp != NULL)
		{
			// Do appropriate action with character
			if (temp->c == NEWLINE)
			{
				if (r < nrows)
				{
					r++;
					c = 0;
				}

				move(r, c);
			}
			else if (temp->c != '\0')
				draw(temp->c);

			// Move down the linked list
			temp = temp->next;
		}
	}

	// Main input loop
	while ((d = getch()) != ESCAPE)
	{
		if (d == KEY_LEFT)
			left();
		else if (d == KEY_RIGHT)
			right();
		else if (d == KEY_UP)
			up();
		else if (d == KEY_DOWN)
			down();
		else if (d == BACKSPACE || d == DELETE || d == KEY_BACKSPACE || d == KEY_DC)
		{
			if (current != buf->head)
			{
				// Remove the previous node
				Node *temp = current;
				current = current->prev;
				remove_node(temp);
				backspace();
			}
		}
		else if (d == NEWLINE || d == KEY_ENTER)
		{
			// Insert node
			current->next = create_node(NEWLINE, current, current->next);
			current = current->next;

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
			current->next = create_node(d, current, current->next);
			current = current->next;
			shiftRestRight();
			draw(d);
		}
	}

	// Close window
	endwin();

	// Ask if user wants to save
	printf("Would you like to save? (y/n): ");
	fflush(stdout);
	char save;
	scanf(" %c", &save);

	if (save == 'y' || save == 'Y')
	{
		// Get the filename
		printf("Enter the filename to write: ");
		fflush(stdout);
		scanf(" %s", filename);
		write_file(filename, buf);
	}

	// Delete buffer
	delete_buffer(buf);
	return 0;
}

// Read data from file into buffer
Node * read_file(char * filename, Buffer * buf)
{
	FILE *fp;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("Error opening file to read\n");
		fflush(stdout);
		exit(1);
	}

	int d;
	Node * current = buf->head;

	while ((d = fgetc(fp)) != EOF)
	{
		current->next = create_node(d, current, NULL);	
		current = current->next;	
	}
	
	fclose(fp);

	return current;
}

// Write buffer to file
void write_file(char * filename, Buffer * buf)
{
	FILE *fp;

	// Open file
	if((fp = fopen(filename, "w+")) == NULL)
	{
		printf("Error occurred open file to save");
		exit(1);
	}

	// Write to file
	Node *temp = buf->head->next;

	while (temp != NULL)
	{
		fputc(temp->c, fp);
		temp = temp->next;
	}

	// Close file
	fclose(fp);
}

// Cursor movements
void left()
{
	if (c == 0)
		skipWhitespace();	
	else
		c--;
	move(r, c);
	current = current->prev;
}

void right()
{
	if (c == ncols)
	{
		if (r != nrows)
		{
			c = 0;
			r++;
		}
	}
	else
	{
		if (current->next->c == NEWLINE)
		{
			if (r != nrows)
			{
				c = 0;
				r++;
			}
		}
		else
		{
			c++;
		}
	}
	move(r, c);
}

void up()
{

}

void down()
{

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
