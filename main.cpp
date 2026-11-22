#include "bowling_frame.h"
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <memory>

int current_game_total(Bowling_Frame* frame)
{
    int total = 0;
    while (frame)
    {
        total += frame->frame_score();
        frame = frame->next_frame();
    }
    return total;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number of players>\n";
        return 1;
    }

    char* end = nullptr;
    errno = 0;
    long count = std::strtol(argv[1], &end, 10);
    if (errno == ERANGE || *end != '\0' || count < 1 || count > INT_MAX)
    {
        std::cerr << "Number of players must be a positive whole number.\n";
        return 1;
    }

    std::unique_ptr<Bowling_Frame*[]> players(new Bowling_Frame*[count]());
    for (int player = 0; player < count; ++player)
    {
        Bowling_Frame* previous = nullptr;
        for (int number = 1; number <= 10; ++number)
        {
            Bowling_Frame* frame = new Bowling_Frame(number);
            frame->prev_frame(previous);
            if (previous)
                previous->next_frame(frame);
            else
                players[player] = frame;
            previous = frame;
        }
    }

    for (int player = 0; player < count; ++player)
    {
        Bowling_Frame* first = players[player];
        for (Bowling_Frame* frame = first; frame; frame = frame->next_frame())
            while (!frame->complete())
                frame->update_scores();

        std::cout << "Player " << player + 1 << '\n';
        for (Bowling_Frame* frame = first; frame; frame = frame->next_frame())
        {
            std::cout << "Frame " << frame->frame_num() << " | Rolls: " << frame->score1();
            if (frame->roll_count() >= 2)
                std::cout << ' ' << frame->score2();
            if (frame->roll_count() == 3)
                std::cout << ' ' << frame->score3();
            std::cout << " | Score: " << frame->frame_score() << '\n';
        }
        std::cout << "Game total: " << current_game_total(first) << "\n\n";

        while (players[player])
        {
            Bowling_Frame* next = players[player]->next_frame();
            delete players[player];
            players[player] = next;
        }
    }
}
