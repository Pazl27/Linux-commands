# Linux commands

[Description](#description)<br>
[Cat](#cat)<br>
[Grep](#grep)<br>
[Ls](#ls)<br>
[Sort](#sort)<br>
[Wc](#wc)<br>



## Description
This is a list of some linux commands that I created in c. The commands are the basic linux commands ls, wc, cat, grep and sort.
The program creates for each of those commands a separate executable file.

## Cat
The cat command reads data from the file and gives their content as output. It is used to read files.
```bash
cat [OPTION] [FILE]
```
### Options
- `-n`: prints the line numbers
- `-v`: prints non-printing characters
- `-E`: prints a $ at the end of each line
***

## Grep
The grep command is used to search for a specific string in a file.
```bash
grep [OPTION] [STRING] [FILE]
```
### Options
- `-i`: case insensitive
- `-v`: prints the lines that do not contain the string
***
## Ls
The ls command lists the files and directories in the current directory.
It also can list the file given to it not only directories.
It sorts the files by default the sort is alphabetical. But it doesn't ignore the . of the hidden files.
If you don't give a file it takes the current directory.
```bash
ls [OPTION] [FILE/DIR]
```
### Options
- `-l`: long listing format
- `-a`: lists all files including hidden files
- `-d`: lists the current directory
***
## Sort
The sort command is used to sort the lines of a file.
It sorts the lines alphabetically by default.
```bash
sort [OPTION] [FILE]
``` 
### Options
- `-r`: reverse the order
***
## Wc
The wc command is used to count the number of lines, words and characters in a file.
By default, it prints the number of lines, words and characters.
```bash
wc [OPTION] [FILE]
```
### Options
- `-l`: prints the number of lines
- `-w`: prints the number of words
- `-c`: prints the number of characters
