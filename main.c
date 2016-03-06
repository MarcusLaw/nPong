/*
Copyright (C) 2016  Marcus Law

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#define center_width screen_width / 2 - 1
#define center_height screen_height / 2 -1

FILE *config;

// Initialization Variables
int screen_width;
int screen_height;

// Loop Variables
int stop;
int input;
int screen;
int counter;
int delay = 64000;

// Game Variables
int ball_x;
int ball_y;
int left_paddle;
int right_paddle;
int x_direction = 1;
int y_direction = 1;
int left_paddle_score;
int right_paddle_score;
int previous_screen;
int cursor_pos;
int winner;

// Options Variables
int max_score = 5;
int left_paddle_height = 5;
int right_paddle_height = 5;
int left_paddle_color = 1;
int right_paddle_color = 1;
int ball_color = 1;
int sound = 1;

// Functions
void mvprintwc(int y, int x, int print_color_pair, const char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    attron(COLOR_PAIR(print_color_pair));
    move(y, x);
    vwprintw(stdscr, Format, Args);
    attroff(COLOR_PAIR(print_color_pair));
    va_end(Args);
}

void start()
{
    screen = 3;
    ball_x = center_width, ball_y = center_height;
    left_paddle_score = 0, right_paddle_score = 0;
    left_paddle = center_height - left_paddle_height / 2;
    right_paddle = center_height - right_paddle_height / 2;
}

void render_paddles()
{
    // Keep Paddles on the Board
    if(left_paddle < 0) left_paddle++;
    if(left_paddle + left_paddle_height > screen_height) left_paddle--;
    if(right_paddle < 0) right_paddle++;
    if(right_paddle + right_paddle_height > screen_height) right_paddle--;

    //Left Paddle
    while(counter < left_paddle_height)
    {
        mvprintwc(left_paddle, 1, left_paddle_color, " ");
        left_paddle++;
        counter++;
    }

    left_paddle = left_paddle - left_paddle_height;
	counter = 0;

    //Right Paddle
    while(counter < right_paddle_height)
    {
        mvprintwc(right_paddle, screen_width - 2, right_paddle_color, " ");
        right_paddle++;
        counter++;
    }

    right_paddle = right_paddle - right_paddle_height;
	counter = 0;
}

// Screens
void main_menu()
{
    //Map Keys
    if(input == KEY_UP) cursor_pos--;
    if(input == KEY_DOWN) cursor_pos++;
    if(input == ' ' && cursor_pos == center_height) start();
    if(input == ' ' && cursor_pos == center_height + 1) cursor_pos = center_height, previous_screen = 0, screen = 1;
    if(input == ' ' && cursor_pos == center_height + 2) stop = true;

    //Keep the Cursor on the List
    if(cursor_pos > center_height + 2) cursor_pos--;
    if(cursor_pos < center_height) cursor_pos++;

    //Draw
    mvprintw(0, 0, "Main Menu");
    mvprintw(cursor_pos, center_width - 2, ">");
    mvprintw(center_height, center_width, "Start");
    mvprintw(center_height + 1, center_width, "Options");
    mvprintw(center_height + 2, center_width, "Quit");
}

void options_menu()
{
    //Map Keys
    if(input == KEY_UP) cursor_pos--;
    if(input == KEY_DOWN) cursor_pos++;
    if(input == KEY_LEFT && cursor_pos == center_height) max_score--;
    if(input == KEY_RIGHT && cursor_pos == center_height) max_score++;
    if(input == KEY_LEFT && cursor_pos == center_height + 1) left_paddle_height--;
    if(input == KEY_RIGHT && cursor_pos == center_height + 1) left_paddle_height++;
    if(input == KEY_LEFT && cursor_pos == center_height + 2) right_paddle_height--;
    if(input == KEY_RIGHT && cursor_pos == center_height + 2) right_paddle_height++;
    if(input == KEY_LEFT && cursor_pos == center_height + 3) left_paddle_color--;
    if(input == KEY_RIGHT && cursor_pos == center_height + 3) left_paddle_color++;
    if(input == KEY_LEFT && cursor_pos == center_height + 4) right_paddle_color--;
    if(input == KEY_RIGHT && cursor_pos == center_height + 4) right_paddle_color++;
    if(input == KEY_LEFT && cursor_pos == center_height + 5) ball_color--;
    if(input == KEY_RIGHT && cursor_pos == center_height + 5) ball_color++;
    if(input == KEY_LEFT && cursor_pos == center_height + 6) sound--;
    if(input == KEY_RIGHT && cursor_pos == center_height + 6) sound++;
    if(input == ' ' && cursor_pos == center_height + 7)
    {
        screen = previous_screen;
        cursor_pos = center_height;
        fprintf(config, "%d %d %d %d %d %d %d", max_score, left_paddle_height, right_paddle_height, left_paddle_color, right_paddle_color, ball_color, sound);
        rewind(config);
    }

    //Limititations
    //Cursor Position
    if(cursor_pos > center_height + 7) cursor_pos--;
    if(cursor_pos < center_height) cursor_pos++;
    //Left Paddle Height
    if(left_paddle_height > screen_height) left_paddle_height--;
    if(left_paddle_height < 1) left_paddle_height++;
    //Right Paddle Height
    if(right_paddle_height > screen_height) right_paddle_height--;
    if(right_paddle_height < 1) right_paddle_height++;
    //Paddle Color
    if(left_paddle_color > 7) left_paddle_color--;
    if(left_paddle_color < 1) left_paddle_color++;
    if(right_paddle_color > 7) right_paddle_color--;
    if(right_paddle_color < 1) right_paddle_color++;
    //Ball Color
    if(ball_color < 1) ball_color++;
    if(ball_color > 7) ball_color--;

    //Render
    mvprintw(0, 0, "Options");
    mvprintw(cursor_pos, center_width - 2, ">");
    //Max Score
    mvprintw(center_height, center_width, "Max Score: %d", max_score);
    //Paddle Height
    mvprintw(center_height + 1, center_width, "Left Paddle Height: %d", left_paddle_height);
    mvprintw(center_height + 2, center_width, "Right Paddle Height: %d", right_paddle_height);
    //Left Paddle Color
    mvprintw(center_height + 3, center_width, "Left Paddle Color:");
    mvprintwc(center_height + 3, center_width + 19, left_paddle_color + 7, "%d", left_paddle_color);
    //Right Paddle Color
    mvprintw(center_height + 4, center_width, "Right Paddle Color:");
    mvprintwc(center_height + 4, center_width + 20, right_paddle_color + 7, "%d", right_paddle_color);
    //Ball Color
    mvprintw(center_height + 5, center_width, "Ball Color:", ball_color);
    mvprintwc(center_height + 5, center_width + 12, ball_color + 7, "%d", ball_color);
    //Sound
    if(sound % 2) mvprintw(center_height + 6, center_width, "Sound: On");
    else mvprintw(center_height + 6, center_width, "Sound: Off");
    //Go Back
    mvprintw(center_height + 7, center_width, "Back");
    //Help Message
    mvprintw(screen_height - 1, center_width, "Left/Right Arrow Keys to Change Values");
}

void pause_menu()
{
    //Map Keys
    if(input == KEY_UP) cursor_pos--;
    if(input == KEY_DOWN) cursor_pos++;
    if(input == ' ' && cursor_pos == center_height) screen = 3;
    if(input == ' ' && cursor_pos == center_height + 1) cursor_pos = center_height, start();
    if(input == ' ' && cursor_pos == center_height + 2) cursor_pos = center_height, previous_screen = 2, screen = 1;
    if(input == ' ' && cursor_pos == center_height + 3) stop = true;

    //Keep the Cusor on the Menu
    if(cursor_pos < center_height) cursor_pos++;
    if(cursor_pos > center_height + 3) cursor_pos--;

    //Draw
    mvprintw(0, 0, "Pause");
    mvprintw(cursor_pos, center_width - 2, ">");
    mvprintw(center_height, center_width, "Resume");
    mvprintw(center_height + 1, center_width, "Restart");
    mvprintw(center_height + 2, center_width, "Options");
    mvprintw(center_height + 3, center_width, "Quit");
}

void run_game()
{
    // Map Keys
    if(input == ' ') screen = 2;
    if(input == 'w') left_paddle--;
    if(input == 's') left_paddle++;
    if(input == KEY_UP) right_paddle--;
    if(input == KEY_DOWN) right_paddle++;

    // Ball Logic
    ball_x += x_direction;
    ball_y += y_direction;
    if(ball_y > screen_height - 2) y_direction = -1;
    if(ball_y < 1) y_direction = 1;

    while(counter < left_paddle_height)
    {
        if(ball_x == 2 && ball_y == left_paddle)
        {
            x_direction = 1;
            if(sound%2) beep();
        }
        left_paddle++;
        counter++;
    }
    left_paddle = left_paddle - left_paddle_height;
	counter = 0;

    while(counter < right_paddle_height)
    {
        if(ball_x == screen_width - 3 && ball_y == right_paddle)
        {
            x_direction = -1;
            if(sound%2) beep();
        }
        right_paddle++;
        counter++;
    }

    right_paddle = right_paddle - right_paddle_height;
	counter = 0;

    if(ball_x < 0)
    {
        right_paddle_score++;
        ball_x = screen_width / 2 - 1;
        ball_y = screen_height / 2 - 1;
        x_direction = 1;
    }

    if(ball_x > screen_width)
    {
        left_paddle_score++;
        ball_x = screen_width / 2 - 1;
        ball_y = screen_height / 2 - 1;
        x_direction = -1;
    }

    //Victory
    if(left_paddle_score > max_score - 1) winner = 1, cursor_pos = center_height, ball_y = screen_height + 1, screen = 4;
    if(right_paddle_score > max_score - 1) winner = 2, cursor_pos = center_height, ball_y = screen_height + 1, screen = 4;

    //Render
    //Scoreboard
    mvprintwc(1, center_width - 1, left_paddle_color + 7, "%d", left_paddle_score);
    mvprintwc(1, center_width, ball_color + 7, "|");
    mvprintwc(1, center_width + 1, right_paddle_color + 7, "%d", right_paddle_score);
	render_paddles();
    //Ball and Score Divider
    mvprintwc(ball_y, ball_x, ball_color + 7, "o");
}

void victory_screen()
{
    //Map Keys
    if(input == KEY_UP) cursor_pos--;
    if(input == KEY_DOWN) cursor_pos++;
    if(input == ' ' && cursor_pos == center_height) cursor_pos = center_height, start();
    if(input == ' ' && cursor_pos == center_height + 1) stop = true;

    //Keep the Cursor on the List
    if(cursor_pos < center_height) cursor_pos++;
    if(cursor_pos > center_height + 1) cursor_pos--;

    //Render
    mvprintw(cursor_pos, center_width - 2, ">");
    if(winner == 1) mvprintwc(center_height - 2, center_width, left_paddle_color + 7, "Left Paddle Wins!");
    if(winner == 2) mvprintwc(center_height - 2, center_width, right_paddle_color + 7, "Right Paddle Wins!");
    mvprintw(center_height, center_width, "Restart");
    mvprintw(center_height + 1, center_width, "Quit");
}

int main(int argc, char *argv[])
{
	// Initialize Ncurses
    initscr();
    noecho();
    cbreak();
    timeout(0);
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, screen_height, screen_width);
    cursor_pos = center_height;

    start_color();
    use_default_colors();
    init_pair(1, -1, COLOR_WHITE);
    init_pair(2, -1, COLOR_RED);
    init_pair(3, -1, COLOR_GREEN);
    init_pair(4, -1, COLOR_YELLOW);
    init_pair(5, -1, COLOR_BLUE);
    init_pair(6, -1, COLOR_MAGENTA);
    init_pair(7, -1, COLOR_CYAN);
    init_pair(8, COLOR_WHITE, -1);
    init_pair(9, COLOR_RED, -1);
    init_pair(10, COLOR_GREEN, -1);
    init_pair(11, COLOR_YELLOW, -1);
    init_pair(12, COLOR_BLUE, -1);
    init_pair(13, COLOR_MAGENTA, -1);
    init_pair(14, COLOR_CYAN, -1);

    config = fopen("config", "r+");
    if(config == NULL) config = fopen("config", "w+");
    fscanf(config, "%d%d%d%d%d%d%d", &max_score, &left_paddle_height, &right_paddle_height, &left_paddle_color, &right_paddle_color, &ball_color, &sound);
    rewind(config);

    while(stop == false)
    {
        clear();
        input = getch();

        switch(screen)
        {
        case 0:
            main_menu();
            break;
        case 1:
            options_menu();
            break;
        case 2:
            pause_menu();
            break;
        case 3:
			run_game();
            break;
        case 4:
            victory_screen();
            break;
        }

        flushinp();
        refresh();
        usleep(delay);
    }

    endwin();
    fclose(config);
    return 0;
}
