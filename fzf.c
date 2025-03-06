#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ncurses.h>
#include <unistd.h>

typedef struct {
    size_t size;
    size_t capacity;
    char *data;
} String;

void initString(String *str, size_t initial_capacity) {
    str->data = malloc(initial_capacity);
    str->size = 0;
    str->capacity = initial_capacity;
    str->data[0] = '\0';
}

void appendString(String *str, char c) {
    if (str->size >= str->capacity-1) {
        str->capacity *= 2;
        str->data = realloc(str->data, str->capacity);
    }
    str->data[str->size] = c;
    str->size++;
    str->data[str->size] = '\0';
}

void popString(String *str) {
    if (str->size >= 1) {
        str->size--;
        str->data[str->size] = '\0';
    }
}

void freeString(String *str) {
    free(str->data);
}

int fzf(const char *subStr, const char *str) {
    while (*subStr != '\0' && *str != '\0') {
        if (*subStr != *str) {
            return 0;
        }
        subStr++;
        str++;
    }
    return 1;
}

int main(int argc, char **argv) {
    String query;
    initString(&query, 8);

    String currentFile;
    initString(&currentFile, 8);

    struct dirent *de;
    DIR *dr;
    char c;

    initscr();
    noecho();
    cbreak();
    curs_set(0);
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    WINDOW *win = newwin(yMax, xMax, 0, 0);
    
    do {
        wclear(win);

        //print first yMax files
        int counter = 0;
        dr = opendir(".");
        while (((de = readdir(dr)) != NULL) && (counter < yMax-2)) {
            if (fzf(de->d_name, query.data)) {
                wmove(win, counter, 0);
                if (counter == 0) {
                    currentFile.size = 0;
                    for (int i = 0; de->d_name[i] != '\0'; i++) {
                        appendString(&currentFile, de->d_name[i]);
                    }
                    wprintw(win, "> %s", currentFile.data);
                } else {
                    wprintw(win, "  %s", de->d_name);
                }
                counter++;
                wrefresh(win);
            }
        }

        wmove(win, yMax-2, 0);
        wprintw(win, "%d", counter);
        wmove(win, yMax-1, 0);
        wprintw(win, "> %s", query.data);
        wrefresh(win);

        c = getch();
        if (c == '\n') {
            if (counter > 0) {
                String command;
                initString(&command, 32);
                snprintf(command.data, command.capacity, "nvim %s", currentFile.data);
                int result = system(command.data);
                if (result == -1) {
                    perror("system() failed");
                }
                freeString(&command);
            }
            freeString(&currentFile);
            break;
        }
        if (c == '\x7f') {
            popString(&query);
        } else {
            appendString(&query, c);
        }
    } while(1);

    freeString(&query);
    closedir(dr);
    endwin();
    curs_set(1);
    return 0;
}
