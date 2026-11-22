#include "bowling_frame.h"
#include <random>

std::random_device dev;
std::mt19937 rng(dev());

Bowling_Frame::Bowling_Frame(int fnum)
    : score1_(0), score2_(0), strike_(false), spare_(false), roll_count_(0),
      score3_(0), frame_num_(fnum), frame_score_(0),
      next_frame_(nullptr), prev_frame_(nullptr)
{
}

// The player list owns the frames. These links only connect neighbors.
Bowling_Frame::~Bowling_Frame() = default;

Bowling_Frame* Bowling_Frame::next_frame() { return next_frame_; }
void Bowling_Frame::next_frame( Bowling_Frame * new_frame ) { next_frame_ = new_frame; }

Bowling_Frame* Bowling_Frame::prev_frame() { return prev_frame_; }
void Bowling_Frame::prev_frame( Bowling_Frame * new_frame ) { prev_frame_ = new_frame; }

int Bowling_Frame::score1()				{ return score1_; }
void Bowling_Frame::score1( int score )	{ score1_ = score; }

int Bowling_Frame::score2()				{ return score2_; }
void Bowling_Frame::score2( int score )	{ score2_ = score; }

bool Bowling_Frame::strike() 			{ return strike_; }
void Bowling_Frame::strike(bool value )	{ strike_ = value; }

bool Bowling_Frame::spare() 			{ return spare_; }
void Bowling_Frame::spare(bool value )	{ spare_ = value; }

int Bowling_Frame::frame_num()
{
	return frame_num_;
}

int Bowling_Frame::frame_score()
{
	return frame_score_; 
}

int Bowling_Frame::roll_count() const { return roll_count_; }
int Bowling_Frame::score3() const { return score3_; }

bool Bowling_Frame::complete() const
{
    if (frame_num_ == 10 && (strike_ || spare_))
        return roll_count_ == 3;
    return strike_ || roll_count_ == 2;
}

void Bowling_Frame::refresh_frame_score()
{
    frame_score_ = score1_ + score2_ + score3_;
    if (frame_num_ == 10)
        return;

    int needed = strike_ ? 2 : (spare_ ? 1 : 0);
    for (Bowling_Frame* next = next_frame_; next && needed > 0; next = next->next_frame_)
    {
        if (next->roll_count_ == 0)
            break;
        frame_score_ += next->score1_;
        --needed;
        if (needed > 0 && next->roll_count_ >= 2)
        {
            frame_score_ += next->score2_;
            --needed;
        }
    }
}

void Bowling_Frame::update_scores()
{
    Bowling_Frame* frame = this;
    while (frame && frame->complete())
        frame = frame->next_frame_;
    if (!frame)
        return;

    int standing = 10;
    if (frame->roll_count_ == 1 && !frame->strike_)
        standing -= frame->score1_;
    if (frame->roll_count_ == 2 && frame->strike_ && frame->score2_ < 10)
        standing -= frame->score2_;

    int pins = std::uniform_int_distribution<int>(0, standing)(rng);
    switch (frame->roll_count_)
    {
        case 0:
            frame->score1_ = pins;
            frame->strike_ = pins == 10;
            break;
        case 1:
            frame->score2_ = pins;
            frame->spare_ = !frame->strike_ && frame->score1_ + pins == 10;
            break;
        case 2:
            frame->score3_ = pins;
            break;
    }
    ++frame->roll_count_;

    // One roll can affect its own frame and the two preceding strikes.
    for (int i = 0; frame && i < 3; ++i, frame = frame->prev_frame_)
        frame->refresh_frame_score();
}
