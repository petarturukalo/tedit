# Description

A simple ncurses terminal text editor with tab handling, editing multiple files (although only one in view
at a time), syntax highlighting, and some basic commands. Access commands by swapping between the current 
file buffer and echo line buffer, where commands are entered, using the escape key.


# Dependent Libraries

* [ncurses](https://invisible-island.net/ncurses/ncurses.html) for display


# Usage

## Starting Up

Start up by running `tedit [filepath ...]` where any number of files can be opened at start up by
providing their filepaths as command line arguments, or none to start editing an empty file, which can
be linked to a new file later on with the write command.

## Syntax Highlighting

Syntax highlighting is enabled for certain file types dependent on file extension. 
Supported file types can be determined by looking at the structures in `src/synhl/rule.c`;
the set of supported types can be easily extended by adding new regex rules and structures here.
  
A current limitation of this syntax highlighting (which I'll get around to fixing eventually) is that 
a substring must be entirely in view for it to be matched and coloured, e.g. if you have a string with 
opening quote in view, and closing quote right and out of view, the part of it that can be seen won't 
be coloured.

## Commands

Enter a command in the echo line buffer with format `cmd [args]` where `cmd` is the name of the command
and `[args]` are arguments (optionally) used by the command.

| Short name | Long name | Description |
| --- | --- | --- |
| w | write | Write the current file buffer to secondary storage. No filepath given has the file buffer's currently linked filepath written to, otherwise if a filepath is given that file is written to, and a new file buffer created (unless the file buffer wasn't yet linked to a filepath). |
| c | close | Close the current file buffer. Requires that the current file buffer not have any unsaved (unwritten) edits. |
| fc | fclose | Force close the current file buffer. Discards any unsaved edits. |
| o | open | Open a file into a new file buffer. Requires a filepath argument of the file to open, otherwise a new unlinked, empty file buffer is opened. If the file doesn't exist then it is created on next write. |
| e | edit | Swap to an already opened file buffer for editing with its name as argument. |
| j | jump | Swap to an already opened file buffer for editing with its ID as argument. |
| ls | list | List all the open file buffers. Listed for each file buffer is "\<filepath\> [\*\<id\>ue]" where \<filepath\> is the filepath linked to the file buffer, or unlinked if it is unlinked, \<id\> is the ID of the file buffer, * is optionally before the ID to identify the current file buffer in view, u and e are optionally after the ID to identify that the file buffer is u[nlinked] or has been e[dited]. |
| q | quit | Quit the text editor. Requires that all open file buffers be saved/written before exiting. |
| fq | fquit | Force quit the text editor. Discards any unsaved edits. |

