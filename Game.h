#pragma once
#include "Map.h"
#include <atomic>
#include <windows.h>
#include <condition_variable>

class Game {
public:
    Game(size_t sz_x, size_t sz_y);
    ~Game();

    int game_loop();

private:
    void render() const;
    void handle_keystroke();
    void handle_timer();
    void reset_cursor() const;
    void place_shape();
    void finish_removal();
    void renew_shape();
    void recalculate_time();
    void game_over();

    std::thread timer;
    std::thread keystroke;
    Shape next_shape;
    Shape shape;
    Map map;
    Map map_next_shape;
    std::chrono::duration<double, std::milli> timer_interval;
    std::chrono::time_point<std::chrono::steady_clock> down_stamp;

    // Main event
    std::mutex event_lock;
    std::condition_variable event_cond;
    bool event_notify;
    bool event_is_timer;
    char event_key;

    // input reply
    std::mutex lock_reply_input;
    std::condition_variable cond_reply_input;

    // timer reply
    std::atomic_bool check_timestamp;

    HANDLE h_console_out;
    HANDLE h_console_in;
    DWORD prev_console_mode;

    std::atomic_bool gaming;

    static constexpr Position shape_base = {4, -2};
    static constexpr Position next_shape_base = {5, 1};

    friend void timer_loop(Game* game);
    friend void keystroke_loop(Game* game);

    int current_lines;
    int lines;
    int score;
    int level;
};

