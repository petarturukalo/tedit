# Description

A simple ncurses terminal text editor with tab handling, editing multiple files (although only one in view
at a time), and some basic commands. Access commands by swapping between the current file buffer and echo line
buffer (where commands are entered) using the escape key.


# Dependent Libraries

* [ncurses](https://invisible-island.net/ncurses/ncurses.html) for display


# Usage

## Starting Up

Start up by running `**tedit** [filepath]...` where any number of files can be opened at start up by
providing their filepaths as command line arguments, or none to start editing an empty file, which can
be linked to a new file later on with the write command.

## Commands

Enter a command in the echo line buffer with format `cmd [args]` where cmdname is the name of the command
and args are arguments (optionally) used by the command.

| Short name | Long name | Description |
| --- | --- | --- |
| w | write | Write the current file buffer to secondary storage. No filepath given has the file buffer's
currently linked filepath written to, otherwise if a filepath is given that file is written to, and a new
file buffer created. |
| c | close | Close the current file buffer. Requires that the current file buffer not have any unsaved 
(unwritten) edits. |
| fc | fclose | Force close the current file buffer. Discards any unsaved edits. |
| o | open | Open a file into a new file buffer. Requires a filepath argument to the file to open. |
| e | edit | Swap to an already opened file buffer for editing with its name as argument. |
| j | jump | Swap to an already opened file buffer for editing with its ID as argument. |
| ls | list | List all the open file buffers. Shown are their names, IDs, and the current file buffer identified
by an asterisk. |
| q | quit | Quit the text editor. Requires that the current file buffer be saved/written before exiting. |
| fq | fquit | Force quit the text editor. Discards any unsaved edits. |

