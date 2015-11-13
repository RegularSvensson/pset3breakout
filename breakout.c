//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// height and width of paddle
#define PHEIGHT (HEIGHT / 60)
#define PWIDTH (WIDTH / 10)

// height and width of bricks
#define WHITESPACE (WIDTH / 100)
#define BHEIGHT 10
#define BWIDTH (WIDTH / COLS - WHITESPACE)

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // randomize initial ball velocity x-axis
    double xVelocity = drand48() * 2.0;
    
    // initial ball velocity y-axis
    double yVelocity = 2.0;
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        // if there was mouse event
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // make paddle follow cursor
                double x = getX(event) - getWidth(paddle) / 2;
                setLocation(paddle, x, HEIGHT - HEIGHT / 5);
            }
        }

        // move ball along angle
        move(ball, xVelocity, yVelocity);
        
        // bounce off top edge of window
        if(getY(ball) <= 0)
        {
            yVelocity = -yVelocity;
        }
        // bounce off right edge of window
        else if(getX(ball) + getWidth(ball) >= getWidth(window))
        {
            xVelocity = -xVelocity;
        }
        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            xVelocity = -xVelocity;
        }
        // bounce off bottom edge of window (for now)
        else if(getY(ball) + RADIUS >= HEIGHT)
        {
            yVelocity = -yVelocity;
        }
        
        // linger before moving again
        pause(10);
        
        // detect collision object with ball
        GObject object = detectCollision(window, ball);
        
        // bounce off paddle
        if (object == paddle)
        {
            // only bounce if positive downward velocity
            if (yVelocity > 0)
            {
                yVelocity = -yVelocity;
            }
        }
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    // array of colors
    char* colors[COLS] = {"RED", "ORANGE", "YELLOW", "GREEN", "BLUE"};
    
    // x and y coordinates
    double x = WHITESPACE / 2;
    double y = WHITESPACE / 2;
    
    // for every row
    for (int i = 0; i < ROWS; i++)
    {
        // for every column
        for (int j = 0; j < COLS; j++)
        {
            // initialize brick
            GRect brick = newGRect(x, y, BWIDTH, BHEIGHT);
            
            // fill rectangle color
            setFilled(brick, true);
            
            // set color of brick
            setColor(brick, colors[i]);
            
            // add brick to window
            add(window, brick);
            
            // increment x coordinate
            x += BWIDTH + WHITESPACE;
        }
        // reset x coordinate
        x = WHITESPACE / 2;
        
        // increment y coordinate
        y += BHEIGHT + WHITESPACE;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
    // initialize ball
    GOval ball = newGOval(WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS, RADIUS, RADIUS);
    
    // fill oval color
    setFilled(ball, true);
    
    // set color to black
    setColor(ball, "BLACK");
    
    // add ball to window
    add(window, ball);
    
    // return GOval ball
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // TODO
    // inititalize paddle
    GRect paddle = newGRect(WIDTH / 2 - PWIDTH / 2, HEIGHT - HEIGHT / 5, PWIDTH, PHEIGHT);
    
    // fill rectangle color
    setFilled(paddle, true);
    
    // set color to black
    setColor(paddle, "BLACK");
    
    // add paddle to window
    add(window, paddle);
    
    // return GRect paddle
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    return NULL;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
