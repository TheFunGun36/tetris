#include "Game.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <conio.h>
#include <mutex>

Game::Game(size_t sz_x, size_t sz_y)
        : map(sz_x, sz_y)
        , map_next_shape(11, 4)
        , shape(shape_base)
        , next_shape(next_shape_base)
        , timer_interval(std::chrono::duration<double, std::milli>(800))
        , current_lines(0)
        , lines(0)
        , score(0)
        , level(0)
        , event_is_timer(false)
        , event_key('\0')
        , event_notify(false) {
    h_console_out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(h_console_out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(h_console_out, &cursorInfo);

    h_console_in = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(h_console_in, &prev_console_mode);
    SetConsoleMode(h_console_in, ENABLE_EXTENDED_FLAGS |
                           (prev_console_mode & ~ENABLE_QUICK_EDIT_MODE));

    next_shape.add_to_map(map_next_shape);
    recalculate_time();
}

Game::~Game() {
    SetConsoleMode(h_console_in, prev_console_mode);
}

void timer_loop(Game *game) {
    do {
        std::this_thread::sleep_for(game->timer_interval);
        if (game->check_timestamp) {
            std::this_thread::sleep_until(game->down_stamp + game->timer_interval);
            game->check_timestamp = false;
        }

        {
            std::lock_guard<std::mutex> lock(game->event_lock);
            game->event_is_timer = true;
            game->event_notify = true;
            game->event_cond.notify_one();
        }
    } while(game->gaming);
}

void keystroke_loop(Game* game) {
    std::unique_lock reply(game->lock_reply_input);

    do {
        {
            char k = char(_getch());  // TODO: Windows only _getch();
            std::lock_guard<std::mutex> lock(game->event_lock);
            game->event_key = k;
            game->event_is_timer = false;
            game->event_notify = true;
            game->event_cond.notify_one();
        }

        game->cond_reply_input.wait(reply);

    } while (game->gaming);
}

int Game::game_loop() {
    gaming = true;
    timer = std::thread(timer_loop, this);
    keystroke = std::thread(keystroke_loop, this);

    {
        auto lock = std::unique_lock(event_lock);

        while (gaming) {
            if (!current_lines)
                shape.add_to_map(map);
            render();
            if (!current_lines)
                shape.remove_from_map(map);

            while(!event_notify) {
                cond_reply_input.notify_one();  // input lock protection
                event_cond.wait(lock);
            }
            event_notify = false;

            if (current_lines) {
                if (event_is_timer)
                    finish_removal();
                else
                    cond_reply_input.notify_one();
            }
            else {
                if (event_is_timer)
                    handle_timer();
                else
                    handle_keystroke();
            }
        }
    }

    keystroke.join();
    timer.join();

    return 0;
}

static void draw_line(size_t cols, bool end=true) {
    std::cout << '+';

    for (size_t i = 0; i < cols; i++)
        std::cout << "--";

    if (end)
        std::cout << '+' << std::endl;
}

static void fit_line(const char* format, size_t value, size_t linewidth) {
    int printed = printf(format, value);
    int to_print = int(linewidth) - printed - 1;
    for (int i = 0; i < to_print; i++) {
        printf(" ");
    }
    printf("|\n");
}

void Game::render() const {
    // Clear
    reset_cursor();

    // Find left-right borders
    int xl = shape[0].x;
    int xr = shape[0].x;
    for (int i = 1; i < Shape::shape_size; i++) {
        if (shape[i].x > xr)
            xr = shape[i].x;
        if (shape[i].x < xl)
            xl = shape[i].x;
    }

    // Draw head
    draw_line(map.cols(), false);
    draw_line(map_next_shape.cols(), true);

    // Draw content
    for (int i = 0; i < map.rows(); i++) {
        // Main body
        std::cout << '|';
        for (int j = 0; j < map.cols(); j++) {
            char c = map.get(j, i);
            char left = (j == xl && c == Map::cell_empty) ? '.' : c;
            char right = (j == xr && c == Map::cell_empty) ? '.' : c;
            std::cout << left << right;
        }

        // Side info
        if (i < map_next_shape.rows()) {
            // Next shape mini-map body
            std::cout << '|';
            const Map::Cell* c = map_next_shape.row(i);
            const Map::Cell* end = map_next_shape.row_end(i);
            while (c < end) {
                std::cout << *c << *c;
                c++;
            }
            std::cout << '|' << std::endl;
        }
        // Data body
        else if (i == map_next_shape.rows())
            draw_line(map_next_shape.cols(), true);
        else if (i == map_next_shape.rows() + 1)
            fit_line("|LEVEL: %d", level, map_next_shape.cols() * 2 + 2);
        else if (i == map_next_shape.rows() + 2)
            fit_line("|SCORE: %d", score, map_next_shape.cols() * 2 + 2);
        else if (i == map_next_shape.rows() + 3)
            fit_line("|LINES: %d", lines, map_next_shape.cols() * 2 + 2);
        else if (i == map_next_shape.rows() + 4)
            draw_line(map_next_shape.cols(), true);
        else
            std::cout << '|' << std::endl;
    }

    draw_line(map.cols());
}

void Game::handle_keystroke() {
    // TODO: remove Windows only keys: Esc and Space

    switch (event_key) {
        // QUIT
        case VK_ESCAPE:
            gaming = false;
            break;

        // SHIFT LEFT/RIGHT
        case 'A':
        case 'a':
            shape.try_shift(map, {-1, 0});
            break;
        case 'D':
        case 'd':
            shape.try_shift(map, {1, 0});
            break;

        // FALL DOWN
        case 'S':
        case 's':
        case VK_SPACE:
            if (shape.try_shift(map, {0, 1})) {
                while (shape.try_shift(map, {0, 1}));
                auto stamp = std::chrono::steady_clock::now();
                down_stamp = stamp;
                check_timestamp = true;
            }

            break;

        // ROTATE COUNTERCLOCKWISE
        case 'Q':
        case 'q':
            shape.try_rotate(map, false);
            break;

        // ROTATE CLOCKWISE
        case 'E':
        case 'e':
            shape.try_rotate(map, true);
            break;
    }

    cond_reply_input.notify_one();
}

void Game::handle_timer() {
    if (!shape.try_shift(map, {0, 1})) {
        place_shape();
    }
}

void Game::reset_cursor() const {
    // TODO: Windows only cursor reset
    SetConsoleCursorPosition(h_console_out, {0, 0});
}

void Game::place_shape() {
    shape.add_to_map(map);

    current_lines = 0;
    for (int i = int(map.rows()) - 1; i >= 0; i--) {
        if (map.is_full(i)) {
            map.fill_row(i, false);
            current_lines++;
        }
    }

    if (!current_lines) {
        if (shape.all_out())
            game_over();
        else
            renew_shape();
    }
}

void Game::game_over() {
    gaming = false;
    cond_reply_input.notify_one();

    system("cls");
    std::cout << "GAME OVER" << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "Lines cleared: " << lines << std::endl;
    std::cout << "Level reached: " << level << std::endl;

    system("pause");
}

void Game::finish_removal() {
    int row_dst = int(map.rows()) - 1;
    int row_src;

    do {
        while (row_dst >= 0 && !map.is_empty(row_dst))
            row_dst--;
        row_src = row_dst - 1;

        while (row_src >= 0 && map.is_empty(row_src))
            row_src--;

        if (row_src >= 0 && row_dst >= 0) {
            map.move_row(row_dst, row_src);
        }
    } while (row_src >= 0);

    lines += current_lines;

    static constexpr int scoring[] = {40, 100, 300, 1200};
    score += scoring[current_lines - 1] * (level + 1);
    current_lines = 0;

    level = lines / 10;

    renew_shape();
    recalculate_time();
}

void Game::renew_shape() {
    next_shape.remove_from_map(map_next_shape);
    next_shape.shift(shape_base - next_shape_base);
    shape = next_shape;
    next_shape = Shape(next_shape_base);
    next_shape.add_to_map(map_next_shape);
}

void Game::recalculate_time() {
    double time = pow(0.8-(level*0.007), level);
    timer_interval = std::chrono::duration<double>(time);
}
