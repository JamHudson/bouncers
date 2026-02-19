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

bn::fixed getSpriteAverageX(bn::vector<bn::sprite_ptr, MAX_BOUNCERS> sprites)
{
    // Add all x positions together
    bn::fixed x_sum = 0;
    for (bn::sprite_ptr sprite : sprites)
    {
        x_sum += sprite.x();
    }

    bn::fixed x_average = x_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (sprites.size() > 0)
    {
        x_average /= sprites.size();
    }

    return x_average;
}

int main()
{
    bn::core::init();

    // Sprites and x speeds of bouncers
    // Items with the same index correspond to each other
    bn::vector<bn::sprite_ptr, MAX_BOUNCERS> sprites = {};
    bn::vector<bn::fixed, MAX_BOUNCERS> x_speeds = {};

    while (true)
    {
        // if A is pressed add a new bouncer
        if (bn::keypad::a_pressed())
        {
            // Only add if we're below the maximum
            if (sprites.size() < sprites.max_size())
            {
                sprites.push_back(bn::sprite_items::dot.create_sprite());
                x_speeds.push_back(BASE_SPEED);
            }
        }

        // if B is pressed print the average to the console
        if (bn::keypad::b_pressed())
        {
            BN_LOG("Average x: ", getSpriteAverageX(sprites));
        }

        // for each bouncer
        for (int i = 0; i < sprites.size(); i++)
        {
            bn::sprite_ptr sprite = sprites[i];

            bn::fixed x = sprite.x();

            // Update x position by adding speed
            x += x_speeds[i];

            // If we've gone off the screen on the right
            if (x > MAX_X)
            {
                // Snap back to screen and reverse direction
                x = MAX_X;
                x_speeds[i] *= -1;
            }
            // If we've gone off the screen on the left
            if (x < MIN_X)
            {
                // Snap back to screen and reverse direction
                x = MIN_X;
                x_speeds[i] *= -1;
            }

            sprite.set_x(x);
        }

        bn::core::update();
    }
}