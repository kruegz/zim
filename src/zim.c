#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include "ascii.h"

typedef struct {
	char * text;
	int size;
} Buffer;

// Keep track of position and size of window
int r, c, nrows, ncols, i;

Buffer * create_buffer(int size);
void read_file(char * filename, Buffer * buf);
void write_file(char * filename, Buffer *buf);
void back();
void forward();
void draw (char dc);
void backspace();
void handleInput(int d, Buffer *buf);

int main(int argc, char ** argv)
{
	// Declare
	int d;
	i = 0;

	// Buffer object
	Buffer *buf = create_buffer(1024); 
	buf->text[0] = '\0';

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
		read_file(filename, buf);
		i--;
	}

	// Setup window
	WINDOW *wnd;
	wnd = initscr();
	cbreak();
	noecho();
	getmaxyx(wnd, nrows, ncols);
	clear();
	refresh();
	
	// Print characters if file was loaded
	if (fileLoaded)
	{
		for (int j = 0; j < i; j++)
		{
			// Only print newlines and characters
			if (buf->text[j] == NEWLINE)
			{
				// Move to beginning of next line
				if (r < nrows)
				{
					r++;
					c = 0;
				}

				move(r, c);
			}
			else if (buf->text[j] != '\0')
			{
				draw(buf->text[j]);
			}
		}	
	}

	while (1)
	{
		d = getch();

		if (d == ESCAPE)
			break;
		else if (d == BACKSPACE || d == DELETE || d == KEY_BACKSPACE || d == KEY_DC)
		{
			i--;
			buf->text[i] = 0;
			backspace();
		}
		else
			handleInput(d, buf);
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

	// Free buffer
	free(buf->text);
	free(buf);
	return 0;
}

// Create and return a buffer
Buffer * create_buffer(int size)
{
	Buffer * buf = malloc(sizeof(Buffer));
	buf->size = size;
	buf->text = calloc(size, sizeof(char));
	return buf;
}

// Read data from file into buffer
void read_file(char * filename, Buffer * buf)
{
	FILE *fp;
	
	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("Error opening file to read");
		fflush(stdout);
		exit(1);
	}
	
	int d;
	
	while ((d = fgetc(fp)) != '\0')
	{
		buf->text[i++] = d;
	}

	fclose(fp);
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
	for (int i = 0; i < buf->size; i++)
	{
		if (buf->text[i] == 3)
			break;
		if (buf->text[i] != 0)
			fputc(buf->text[i], fp);
	}
	fputc(3, fp);
	fputc('\0', fp);

	// Close file
	fclose(fp);
}

// Navigation functions with overflow control
void back()
{
	if (c == 0)
	{
		if (r != 0)
		{
			c = ncols;
			r--;
		}
	}
	else
		c--;
}

void forward()
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
		c++;
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

// Perform a backspace
void backspace()
{
	// Check to see if we're deleting a newline
	if (c == 0)
	{	
		if (r != 0)
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

void handleInput(int d, Buffer *buf)
{
	if (d == 10 || d == KEY_ENTER)
	{
		if (i < buf->size)
		{
			if (buf->text[i] == '\0')
				buf->text[i+1] = '\0';
			buf->text[i] = NEWLINE;
			i++;

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
			exit(1);
		}
	}
	else
	{
		if (i < buf->size)
		{
			if (buf->text[i] == '\0')
				buf->text[i+1] = '\0';
			buf->text[i] = d;
			draw(d);
			i++;
		}
		else
		{
			exit(1);
		}
	}
}
