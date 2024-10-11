#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define FIELD_WIDTH 50
#define FIELD_HEIGHT 16

#define SNAKE_HEAD "&"
#define SNAKE_BODY "@"
#define APPLE "o"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    initscr();
    curs_set(0);
    noecho();
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    
    // field drawing
    attron(COLOR_PAIR(1));
    for (int i = 0; i <= FIELD_HEIGHT; i++)
    {
        for (int j = 0; j <= FIELD_WIDTH; j++)
        {
            move(i, j);
            if (j == 0 || j == FIELD_WIDTH)
                printw("|");
            else if (i == 0 || i == FIELD_HEIGHT)
                printw("-");
        }
    }
    attroff(COLOR_PAIR(1));

    move(FIELD_HEIGHT + 1, FIELD_WIDTH / 2 - 3);
    addstr("SCORE: 0");

    // initialization
    int score = 0;
    int dx = 1, dy = 0;
    int length = 2;
    int *x_nex = (int*)malloc(length * sizeof(int));
    int *y_nex = (int*)malloc(length * sizeof(int));
    bool break_flag = 0;

    for (int i = 0; i < length; i++) // initializing the coordinates of the snake
    {
        x_nex[i] = FIELD_WIDTH / 2 - i;
        y_nex[i] = FIELD_HEIGHT / 2;
    }

    // snake drawing
    move(y_nex[0], x_nex[0]);
    attron(COLOR_PAIR(2));
    printw(SNAKE_HEAD);
    for (int i = 1; i < length; i++)
    {
        move(y_nex[i], x_nex[i]);
        printw(SNAKE_BODY);
    }
    attroff(COLOR_PAIR(2));

    // apple drawing
    int apple_x, apple_y;
    bool apple_flag;
    do
    {
        apple_flag = 1;

        apple_x = 1 + rand() % (FIELD_WIDTH - 1);
        apple_y = 1 + rand() % (FIELD_HEIGHT - 1);

        for (int i = 0; i < length; i++)
        {
            if (apple_x == x_nex[i] && apple_y == y_nex[i])
                apple_flag = 0;
        }
    } while (apple_flag != 1);
    move(apple_y, apple_x);
    attron(COLOR_PAIR(3));
    printw(APPLE);
    attroff(COLOR_PAIR(3));
    
    while (break_flag != 1) // game cycle
    {
        usleep(100000);
        nodelay(stdscr, 1);

        switch (getch()) // reading keyboard
        {
        // handling arrow buttons
        // case 259:
        //     if (dy == -1)
        //         break;
        //     dx = 0;
        //     dy = 1;
        //     break;
        // case 258:
        //     if (dy == 1)
        //         break;
        //     dx = 0;
        //     dy = -1;
        //     break;
        // case 260:
        //     if (dx == 1)
        //         break;
        //     dx = -1;
        //     dy = 0;
        //     break;
        // case 261:
        //     if (dx == -1)
        //         break;
        //     dx = 1;
        //     dy = 0;
        //     break;
        
        // handling russian keyboard
        case 208:
            switch (getch())
            {
            case 178:
                if (dx == -1)
                    break;
                dx = 1;
                dy = 0;
                break;
            default:
                break;
            }
            break;
        
        case 209:
            switch (getch())
            {
            case 139:
                if (dy == -1)
                    break;
                dx = 0;
                dy = 1;
                break;
            case 134:
                if (dy == 1)
                    break;
                dx = 0;
                dy = -1;
                break;
            case 132:
                if (dx == 1)
                    break;
                dx = -1;
                dy = 0;
                break;
            default:
                break;
            }
            break;
        
        // handling english keyboard
        case 115:
            if (dy == -1)
                break;
            dx = 0;
            dy = 1;
            break;
        
        case 119:
            if (dy == 1)
                break;
            dx = 0;
            dy = -1;
            break;
        
        case 97:
            if (dx == 1)
                break;
            dx = -1;
            dy = 0;
            break;
        
        case 100:
            if (dx == -1)
                break;
            dx = 1;
            dy = 0;
            break;
        
        default:
            break;
        }

        // tail grouting
        move(y_nex[length - 1], x_nex[length - 1]);
        printw(" ");

        for (int i = length - 1; i > 0; i--) // setting body coordinates
        {
            x_nex[i] = x_nex[i - 1];
            y_nex[i] = y_nex[i - 1];
        }

        // setting head coordinates
        x_nex[0] += dx;
        y_nex[0] += dy;

        for (int i = 1; i < length; i++) // checking for reaching the body
        {
            if (x_nex[0] == x_nex[i] && y_nex[0] == y_nex[i])
                break_flag = 1;
        }

        if (x_nex[0] == apple_x && y_nex[0] == apple_y) // checking for reaching an apple
        {
            length++;
            score++;

            move(FIELD_HEIGHT + 1, FIELD_WIDTH / 2 + 4);
            printw("%d", score);

            x_nex = (int*)realloc(x_nex, length * sizeof(int));
            y_nex = (int*)realloc(y_nex, length * sizeof(int));

            x_nex[length - 1] = x_nex[length - 2] - dx;
            y_nex[length - 1] = y_nex[length - 2] - dy;

            do
            {
                apple_flag = 1;
        
                apple_x = 1 + rand() % (FIELD_WIDTH - 1);
                apple_y = 1 + rand() % (FIELD_HEIGHT - 1);

                for (int i = 0; i < length; i++)
                {
                    if (apple_x == x_nex[i] && apple_y == y_nex[i])
                        apple_flag = 0;
                }
            } while (apple_flag != 1);

            move(apple_y, apple_x);
            attron(COLOR_PAIR(3));
            printw(APPLE);
            attroff(COLOR_PAIR(3));
        }

        // checking for reaching the bounds
        if (x_nex[0] == FIELD_WIDTH)
            x_nex[0] = 1;
        if (x_nex[0] == 0)
            x_nex[0] = FIELD_WIDTH - 1;
        if (y_nex[0] == FIELD_HEIGHT)
            y_nex[0] = 1;
        if (y_nex[0] == 0)
            y_nex[0] = FIELD_HEIGHT - 1;
        
        // drawing parts of the snake with changed coordinates
	attron(COLOR_PAIR(2));
        move(y_nex[0], x_nex[0]);
        printw(SNAKE_HEAD);
        move(y_nex[1], x_nex[1]);
        printw(SNAKE_BODY);
	move(y_nex[length - 1], x_nex[length - 1]);
        printw(SNAKE_BODY);
        attroff(COLOR_PAIR(2));
    }
    endwin();
}
