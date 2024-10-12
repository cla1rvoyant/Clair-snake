#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define FIELD_WIDTH 50
#define FIELD_HEIGHT 16
#define INITIAL_LENGTH 2

#define SNAKE_HEAD "&"
#define SNAKE_BODY "@"
#define APPLE "o"

int apple_x, apple_y;
bool break_flag = 0;

typedef struct Nexus
{
    struct Nexus *next;
    struct Nexus *prev;
    int x;
    int y;
} Nexus;

typedef struct Snake
{
    Nexus *head;
    Nexus *tail;
} Snake;

void add_head(Snake *snake, int x_coord, int y_coord)
{
    Nexus *newNexus = (Nexus*)malloc(sizeof(Nexus));

    newNexus->prev = newNexus->next = NULL;
    newNexus->x = x_coord;
    newNexus->y = y_coord;

    if (snake->head)
    {
        snake->head->prev = newNexus;
        newNexus->next = snake->head;
        snake->head = newNexus;
    }
    else 
        snake->head = snake->tail = newNexus;
}

void add_tail(Snake *snake, int x_coord, int y_coord)
{
    Nexus *newNexus = (Nexus*)malloc(sizeof(Nexus));
  
    newNexus->prev = newNexus->next = NULL;
    newNexus->x = x_coord;
    newNexus->y = y_coord;

    snake->tail->next = newNexus;
    newNexus->prev = snake->tail;
    snake->tail = newNexus;
}

void delete_tail(Snake *snake)
{
    Nexus *tailNexus = snake->tail;
    tailNexus->prev->next = tailNexus->next;
    snake->tail = tailNexus->prev;
    free(tailNexus);
}

void generate_snake(Snake *snake)
{
    add_head(snake, FIELD_WIDTH / 2, FIELD_HEIGHT / 2);
    for (int i = 1; i < INITIAL_LENGTH; i++) // initializing the coordinates of the snake
        add_tail(snake, FIELD_WIDTH / 2 - i, FIELD_HEIGHT / 2);
    attron(COLOR_PAIR(2));
    Nexus *current = snake->head;
    while (current)
    {
        move(current->y, current->x);
        if (current == snake->head)
            printw(SNAKE_HEAD);
        else
            printw(SNAKE_BODY);
        current = current->next;
    }
    attroff(COLOR_PAIR(2));
}

void generate_apple(Snake snake)
{
    bool apple_flag;
    do
    {
        apple_flag = 1;

        apple_x = 1 + rand() % (FIELD_WIDTH - 1);
        apple_y = 1 + rand() % (FIELD_HEIGHT - 1);

        Nexus *current = snake.head;
        while (current)
        {
            if (apple_x == current->x && apple_y == current->y)
                apple_flag = 0;
            current = current->next;
        }
    } while (apple_flag != 1);
    attron(COLOR_PAIR(3));
    move(apple_y, apple_x);
    printw(APPLE);
    attroff(COLOR_PAIR(3));
}

void self_eating_check(Snake snake)
{
    Nexus *current = snake.head->next;
    while (current)
    {
        if (snake.head->x == current->x && snake.head->y == current->y)
            break_flag = 1;
        current = current->next;
    }
}

void reaching_bounds_check(Snake snake)
{
    if (snake.head->x == FIELD_WIDTH)
        snake.head->x = 1;
    if (snake.head->x == 0)
        snake.head->x = FIELD_WIDTH - 1;
    if (snake.head->y == FIELD_HEIGHT)
        snake.head->y = 1;
    if (snake.head->y == 0)
        snake.head->y = FIELD_HEIGHT - 1;
}

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
    Snake snake;
    
    generate_snake(&snake);

    generate_apple(snake);
    
    while (break_flag != 1) // game cycle
    {
        usleep(100000);
        nodelay(stdscr, 1);

        switch (getch()) // reading keyboard
        {
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
        
        // setting head coordinates
        add_head(&snake, snake.head->x + dx, snake.head->y + dy);

        // apple reaching check
        if (snake.head->x == apple_x && snake.head->y == apple_y)
        {
            score++;
            move(FIELD_HEIGHT + 1, FIELD_WIDTH / 2 + 4);
            printw("%d", score);

            generate_apple(snake);
        }
        else
        {
            move(snake.tail->y, snake.tail->x);
            printw(" ");
            delete_tail(&snake);
        }

        self_eating_check(snake);

        reaching_bounds_check(snake);
        
        // drawing parts of the snake with changed coordinates
        attron(COLOR_PAIR(2));
        move(snake.head->y, snake.head->x);
        printw(SNAKE_HEAD);
        move(snake.head->next->y, snake.head->next->x);
        printw(SNAKE_BODY);
        attroff(COLOR_PAIR(2));
    }
    endwin();
}