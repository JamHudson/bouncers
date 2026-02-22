#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>
#include <bn_random.h>
#include <bn_math.h>

#include "bn_sprite_items_dot.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;

// Set max/min y position to be the TOP edges of display
static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Random
static bn::random rng = bn::random();

// Starting speed of a bouncer
static constexpr bn::fixed BASE_SPEED = 2;

// Physics decrease
static constexpr bn::fixed BASE_GRAVITY = 3;
static constexpr bn::fixed BASE_FRICTION = 0.25;   // 0 <= x < 1 for realistic behavior.
static constexpr bn::fixed BASE_ELASTICITY = 0.65; // 0 <= x < 1 for realistic behavior.

// Whether or not physics is active
static bool physics = false;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

static constexpr bn::fixed MAX_SPEED = 10.0;
static constexpr bn::fixed MIN_SPEED = -10.0;

static constexpr bn::color defaultBackground = bn::color(31, 0, 15);
static constexpr bn::color physicsBackground = bn::color(0, 18, 31);

class Bouncer
{
public:
    bn::sprite_ptr sprite = bn::sprite_items::dot.create_sprite();
    bn::fixed x_speed = 0;
    bn::fixed y_speed = 0;
    bn::fixed top_x_speed = rng.get_fixed(MIN_SPEED, MAX_SPEED);
    bn::fixed top_y_speed = rng.get_fixed(MIN_SPEED, MAX_SPEED);

    // Sets bouncer to its default speed.
    void setNormalSpeed()
    {

        x_speed = top_x_speed;
        y_speed = top_y_speed;
    }

    void update()
    {
        bn::fixed x = sprite.x();
        bn::fixed y = sprite.y();

        if (physics)
        {
            // Decrease x speed to 0, decrease y speed to max.
            // Only reduced speed if there is any, prevents division by 0
            if (x_speed == 0)
            {
                x_speed = 0;
            }
            else
            {
                // MAX_Y is the bottom
                if (y == MAX_Y)
                {
                    x_speed -= x_speed * BASE_FRICTION;
                    if (bn::abs(x_speed) < 1)
                    {
                        x_speed = 0;
                    }
                }
            }
            y_speed += BASE_GRAVITY;
        }
        // Update x position by adding speed
        x += x_speed;
        y += y_speed;

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

        // If we've gone off the screen on the bottom
        if (y > MAX_Y)
        {
            // Snap back to screen and reverse direction
            y = MAX_Y;
            y_speed *= -1;
            // If physics are enabled, use an elastic collision (but only for floor).
            if (physics && y_speed != 0)
            {
                y_speed *= BASE_ELASTICITY;
            }
        }
        // If we've gone off the screen on the top
        if (y < MIN_Y)
        {
            // Snap back to screen and reverse direction
            y = MIN_Y;
            y_speed *= -1;
        }
        sprite.set_x(x);
        sprite.set_y(y);
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
        Bouncer b = Bouncer();
        b.setNormalSpeed();
        bouncers.push_back(b);
    }
}

int main()
{
    bn::core::init();
    bn::backdrop::set_color(defaultBackground);

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

        // if R is pressed, drop all of the bouncers.
        if (bn::keypad::r_pressed())
        {
            physics = !physics;
            if (!physics)
            {
                bn::backdrop::set_color(defaultBackground);
                for (Bouncer &bouncer : bouncers)
                {
                    bouncer.setNormalSpeed();
                }
            }
            else
            {
                bn::backdrop::set_color(physicsBackground);
            }
        }

        // for each bouncer
        for (Bouncer &bouncer : bouncers)
            bouncer.update();

        bn::core::update();
    }
}