#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>

#include "bn_sprite_items_dot.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;

// Starting speed of a bouncer
static constexpr bn::fixed BASE_SPEED = 2;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer
{
public:
    bn::sprite_ptr sprite = bn::sprite_items::dot.create_sprite();
    bn::fixed x_speed = BASE_SPEED;

    void update()
    {
        bn::fixed x = sprite.x();

        // Update x position by adding speed
        x += x_speed;

        // If we've gone off the screen on the right
        if (x > MAX_X)
        {
            // Snap back to screen and reverse direction
            x = MAX_X;
            x_speed *= -1;
        }
        // If we've gone off the screen on the left
        if (x < MIN_X)
        {
            // Snap back to screen and reverse direction
            x = MIN_X;
            x_speed *= -1;
        }

        sprite.set_x(x);
    }
};

bn::fixed getAverageX(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    // Add all x positions together
    bn::fixed x_sum = 0;
    for (Bouncer bouncer : bouncers)
    {
        x_sum += bouncer.sprite.x();
    }

    bn::fixed x_average = x_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (bouncers.size() > 0)
    {
        x_average /= bouncers.size();
    }

    return x_average;
}

void addBouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    // Only add if we're below the maximum
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer());
    }
}

int main()
{
    bn::core::init();

    // Bouncers
    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    while (true)
    {
        // if A is pressed add a new bouncer
        if (bn::keypad::a_pressed())
        {
            addBouncer(bouncers);
        }

        // if B is pressed print the average to the console
        if (bn::keypad::b_pressed())
        {
            BN_LOG("Average x: ", getAverageX(bouncers));
        }

        // for each bouncer
        for (Bouncer &bouncer : bouncers)
            bouncer.update();

        bn::core::update();
    }
}