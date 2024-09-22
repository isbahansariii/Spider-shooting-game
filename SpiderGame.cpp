#include <gl/glut.h>
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "pixMap.h"

using namespace std;

RGBApixmap backgroundPixmap;
int screenWidth = 940;
int screenHeight = 580;
bool gameOver = false; // Flag to indicate game over

class Point2
{
public:
    float x, y;
    Point2() : x(0), y(0) {}
    Point2(float xx, float yy) : x(xx), y(yy) {}
    float getX() { return x; }
    float getY() { return y; }
};

class Spider
{
private:
    enum State
    {
        ALIVE,
        DEAD
    } state;
    RGBApixmap pix[2];
    float directionX, directionY; // Direction of movement
public:
    float pos_X, pos_Y;
    bool isMoving;
    float spiderSpeed;
    Spider() {}

    Spider(Point2 pos)
    {
        isMoving = true;
        spiderSpeed = 0.35f; // Movement speed
        this->pos_X = pos.getX();
        this->pos_Y = pos.getY();
        directionX = 0.5f; // Initial direction (moving right)
        directionY = 1.0f; // Initial direction (moving up)

        string fname[2] = {"spider.bmp", "mak3.bmp"}; // Replace with actual image filenames
        for (int i = 0; i < 2; i++)
        {
            pix[i].readBMPFile(fname[i], 1);
            pix[i].setChromaKey(255, 255, 255);
        }
    }

    void setState(int i)
    {
        if (i == 1)
            this->state = DEAD;
        else
        {
            this->state = ALIVE;
        }
    }

    int getState()
    {
        return (this->state == DEAD) ? 1 : 0;
    }

    void changePosition(float dx, float dy)
    {
        this->pos_X += dx;
        this->pos_Y += dy;
    }

    void die()
    {
        this->isMoving = false;
        this->state = DEAD;
    }

    void render()
    {
        if (gameOver)
            return; // Don't render spider after Game Over

        glRasterPos2f(this->pos_X, this->pos_Y);
        if (this->isMoving)
        {
            pix[0].mDraw();
        }
        else
        {
            pix[1].mDraw();
        }
    }

    void moveSpider()
    {
        if (!isMoving || gameOver)
            return; // Don't move if game is over

        // Update position with constant movement
        pos_X += directionX * spiderSpeed;
        pos_Y += directionY * spiderSpeed;

        // Bounce off the screen edges
        if (pos_X < 0 || pos_X > screenWidth - 50)
        {
            directionX = -directionX; // Reverse X direction
        }
        if (pos_Y < 0 || pos_Y > screenHeight - 50)
        {
            directionY = -directionY; // Reverse Y direction
        }
    }
};

Spider spider(Point2(300, 220));

bool isBulletFired = false;
bool isBulletMoving = false;
float bulletX = screenWidth / 2;
float bulletY = 40.0f;
float bulletSpeed = 8.0f;

void updateBullet()
{
    if (isBulletMoving)
    {
        bulletY += bulletSpeed;
        if (bulletY >= screenHeight)
        {
            isBulletMoving = false;
        }
    }
}

class Bullet
{
public:
    float x, y;
    Bullet(float initialX, float initialY) : x(initialX), y(initialY) {}

    void render()
    {
        if (gameOver)
            return; // Don't render bullet after Game Over

        glPushMatrix();
        glTranslatef(x, y, 0.0);
        // glColor3f(0.5, 0.1, 0.2);
        glColor3f(1.0, 1.0, 1.0);

        glBegin(GL_TRIANGLES);
        glVertex2f(0, 0);
        glVertex2f(-3, -5);
        glVertex2f(3, -5);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex2f(3, -5);
        glVertex2f(-3, -5);
        glVertex2f(-3, -15);
        glVertex2f(3, -15);
        glEnd();
        glPopMatrix();
    }

    void setPosition(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    void changePosition(float dx, float dy)
    {
        x += dx;
        y += dy;
    }
};

Bullet bullet(bulletX, bulletY);

void myKB_Handler(unsigned char key, int mx, int my)
{
    if (gameOver)
        return; // No input after Game Over

    switch (key)
    {
    case ' ':
        if (!isBulletFired)
        {
            isBulletFired = true;
            isBulletMoving = true;
            bullet.changePosition(0, 20);
        }
        break;
    case 'a': // Move bullet left
        if (!isBulletMoving)
        {
            bullet.changePosition(-10, 0); // Adjust this value for desired speed
        }
        break;
    case 'd': // Move bullet right
        if (!isBulletMoving)
        {
            bullet.changePosition(10, 0); // Adjust this value for desired speed
        }
        break;
    }
    glutPostRedisplay();
}

void bullet_movement(int y)
{
    bullet.changePosition(0, 20);
    glutPostRedisplay();
    if ((bullet.x >= spider.pos_X && bullet.x <= spider.pos_X + 50) &&
        (bullet.y >= spider.pos_Y && bullet.y <= spider.pos_Y + 50))
    {
        bullet.setPosition(bulletX, 20);
        spider.die();
        isBulletMoving = false;
        gameOver = true; // Set game over flag
        glutPostRedisplay();
    }
    if (bullet.y < 480 && isBulletMoving)
    {
        glutTimerFunc(5, bullet_movement, y);
    }
    else if (bullet.y >= 480)
    {
        bullet.setPosition(bulletX, 20);
        glutPostRedisplay();
    }
}

void pressKeySpecial(int key, int x, int y)
{
    if (gameOver)
        return; // No input after Game Over

    if (key == GLUT_KEY_UP && !isBulletMoving)
    {
        isBulletMoving = true;
        isBulletFired = true;
        bullet_movement(0);
        PlaySound(TEXT("sounds\\shoot.wav"), NULL, SND_ASYNC);
    }
}

void displayGameOver()
{
    if (gameOver)
    {
        glColor3f(1.0, 1.0, 1.0); // white color for "Game Over"
        glRasterPos2f(screenWidth / 2 - 50, screenHeight / 2);
        const char *msg = "Game Over!";
        for (int i = 0; msg[i] != '\0'; i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg[i]);
        }
    }
}

void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glRasterPos2f(0, 0); // Position to start drawing the background image
    backgroundPixmap.mDraw();
    if (!gameOver)
    {
        updateBullet();
        spider.moveSpider(); // Continuous spider movement
        spider.render();
        bullet.render();
    }
    displayGameOver(); // Display "Game Over" when the game ends
    glFlush();
}

void myInit(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_EQUAL, 1.0);

    // Initialize background image
    string bgFilename = "bgFull.bmp"; // Replace with your background image file
    backgroundPixmap.readBMPFile(bgFilename, 1);
    backgroundPixmap.setChromaKey(500, 0, 255); // Set chroma key if necessary
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Spider Game");

    glutDisplayFunc(myDisplay);
    glutSpecialFunc(pressKeySpecial);
    glutKeyboardFunc(myKB_Handler);

    myInit();
    glutIdleFunc(myDisplay); // Ensure the game keeps updating
    PlaySound(TEXT("sounds\bgSound.wav"), NULL, SND_ASYNC|SND_LOOP);

    glutMainLoop();
}
