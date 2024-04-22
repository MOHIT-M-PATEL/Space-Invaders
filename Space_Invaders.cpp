#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

const int screenWidth = 1500;
const int screenHeight = 700;
const int playerWidth = 50;
const int playerHeight = 20;
const int alienRows = 5;
const int alienCols = 10;
const int alienWidth = 30;
const int alienHeight = 30; // Increased alien height for better appearance
const int bulletWidth = 3;
const int bulletHeight = 10;
const int playerSpeed = 5;
const int bulletSpeed = 10;

struct Player {
    int x, y;
};

struct Bullet {
    int x, y;
    bool active;
};

struct Alien {
    int x, y;
    bool alive;
};

void drawPlayer(Player &player) {
    line(player.x, player.y + playerHeight, player.x + playerWidth / 2, player.y);
    line(player.x + playerWidth / 2, player.y, player.x + playerWidth, player.y + playerHeight);
    circle(player.x + playerWidth / 2, player.y + playerHeight / 2, playerWidth / 4);
}

void drawBullet(Bullet &bullet) {
    rectangle(bullet.x, bullet.y, bullet.x + bulletWidth, bullet.y + bulletHeight);
}

void drawAliens(Alien aliens[alienRows][alienCols], int numAliens) {
    for (int i = 0; i < numAliens; i++) {
        if (aliens[i / alienCols][i % alienCols].alive) {
            setfillstyle(SOLID_FILL, YELLOW); // Set alien color to yellow
            bar(aliens[i / alienCols][i % alienCols].x, aliens[i / alienCols][i % alienCols].y,
                aliens[i / alienCols][i % alienCols].x + alienWidth, aliens[i / alienCols][i % alienCols].y + alienHeight);
        }
    }
}

void movePlayer(Player &player, int direction) {
    if (direction == 0 && player.x > 0) {
        player.x -= playerSpeed;  // Move left
    } else if (direction == 1 && player.x + playerWidth < screenWidth) {
        player.x += playerSpeed;  // Move right
    }
}

void moveBullet(Bullet &bullet) {
    if (bullet.active) {
        bullet.y -= bulletSpeed;
        if (bullet.y <= 0) {
            bullet.active = false;
        }
    }
}

void moveAliens(Alien aliens[alienRows][alienCols], int numAliens, int level) {
    static int direction = 1; // 1 for right, -1 for left
    static int alienSpeedCounter = 0;
    int alienSpeed = 3 + level; // Increase speed based on level

    alienSpeedCounter++;
    if (alienSpeedCounter >= alienSpeed) {
        alienSpeedCounter = 0;
        for (int i = 0; i < numAliens; i++) {
            aliens[i / alienCols][i % alienCols].x += 10 * direction * level; // Increase movement based on level
            if (aliens[i / alienCols][i % alienCols].x <= 0 || aliens[i / alienCols][i % alienCols].x + alienWidth >= screenWidth) {
                direction = -direction;
                for (int j = 0; j < numAliens; j++) {
                    aliens[j / alienCols][j % alienCols].y += alienHeight;
                }
                return;
            }
        }
    }
}


bool checkCollision(Bullet &bullet, Alien aliens[alienRows][alienCols], int numAliens) {
    for (int i = 0; i < numAliens; i++) {
        if (aliens[i / alienCols][i % alienCols].alive &&
            bullet.x + bulletWidth >= aliens[i / alienCols][i % alienCols].x &&
            bullet.x <= aliens[i / alienCols][i % alienCols].x + alienWidth &&
            bullet.y <= aliens[i / alienCols][i % alienCols].y + alienHeight &&
            bullet.y + bulletHeight >= aliens[i / alienCols][i % alienCols].y) {
            aliens[i / alienCols][i % alienCols].alive = false;
            return true;
        }
    }
    return false;
}

bool checkGameOver(Alien aliens[alienRows][alienCols], int numAliens) {
    for (int i = 0; i < numAliens; i++) {
        if (aliens[i / alienCols][i % alienCols].alive && aliens[i / alienCols][i % alienCols].y + alienHeight >= screenHeight - playerHeight) {
            return true;
        }
    }
    return false;
}

void drawGameOver() {
    setcolor(RED);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    outtextxy(screenWidth / 4, screenHeight / 2, "GAME OVER");
}

void drawLevel(int level, bool cleared) {
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4);
    char levelText[20];
    if (!cleared) {
        sprintf(levelText, "LEVEL %d", level);
    } else {
        sprintf(levelText, "LEVEL CLEARED");
    }
    outtextxy(screenWidth / 4, screenHeight / 2, levelText);

    if (cleared) {
        settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
        outtextxy(screenWidth / 4, screenHeight / 2 + 50, "Next Level Starting...");
    }
}

int main() {
    initwindow(1500, 700);

    Player player = {screenWidth / 2 - playerWidth / 2, screenHeight - playerHeight - 20};
    Bullet bullets[10];  // Array to hold multiple bullets
    Alien aliens[alienRows][alienCols];

    int level = 1; // Initial level
    bool levelCleared = false;  // Set to true when level is cleared

    // Initialize aliens based on level
    srand(time(NULL));
    int numAliens = 10 + level * 5;  // Initial number of aliens
    for (int i = 0; i < numAliens; i++) {
        aliens[i / alienCols][i % alienCols].x = (i % alienCols) * (alienWidth + 10) + 50;
        aliens[i / alienCols][i % alienCols].y = (i / alienCols) * (alienHeight + 10) + 50;
        aliens[i / alienCols][i % alienCols].alive = true;
    }

    char key = '\0';  // Initialize key variable outside the loop

    while (key != 'q' && !checkGameOver(aliens, numAliens)) {
        cleardevice();

        // Handle keyboard input
        if (kbhit()) {
            key = getch();
            if (key == 'a' || key == 'A') {
                movePlayer(player, 0);  // Move left
            } else if (key == 'd' || key == 'D') {
                movePlayer(player, 1);  // Move right
            } else if (key == ' ' || key == 's' || key == 'S') {  // Added 's' as a new key to fire continuously
                for (int i = 0; i < 10; i++) {  // Check for an inactive bullet slot
                    if (!bullets[i].active) {
                        bullets[i].active = true;
                        bullets[i].x = player.x + playerWidth / 2 - bulletWidth / 2;
                        bullets[i].y = player.y - bulletHeight;
                        break;  // Exit loop after firing a bullet
                    }
                }
            }
        }

        // Update bullet positions
        for (int i = 0; i < 10; i++) {  // Update all bullets
            moveBullet(bullets[i]);
        }

        // Draw game elements
        drawPlayer(player);
        for (int i = 0; i < 10; i++) {  // Draw all active bullets
            if (bullets[i].active) {
                drawBullet(bullets[i]);
            }
        }
        drawAliens(aliens, numAliens);

        // Move aliens and handle collisions
        moveAliens(aliens, numAliens, level);
        for (int i = 0; i < 10; i++) {  // Check collisions for all active bullets
            if (bullets[i].active && checkCollision(bullets[i], aliens, numAliens)) {
                bullets[i].active = false;
                // Increment score or perform other actions
            }
        }

        // Check for game over
        if (checkGameOver(aliens, numAliens)) {
            drawGameOver();
            break;
        }

        // Check if all aliens are cleared
        bool allAliensCleared = true;
        for (int i = 0; i < numAliens; i++) {
            if (aliens[i / alienCols][i % alienCols].alive) {
                allAliensCleared = false;
                break;
            }
        }

        if (allAliensCleared) {
            levelCleared = true;
            delay(3000);  // Wait for 3 seconds
            level++;  // Increase level
            // Reset aliens and other game state for the next level
            numAliens = 10 + level * 5;  // Increase the number of aliens based on level
            for (int i = 0; i < numAliens; i++) {
                aliens[i / alienCols][i % alienCols].x = (i % alienCols) * (alienWidth + 10) + 50;
                aliens[i / alienCols][i % alienCols].y = (i / alienCols) * (alienHeight + 10) + 50;
                aliens[i / alienCols][i % alienCols].alive = true;
            }
            levelCleared = false;  // Reset level cleared flag
        }

        drawLevel(level, levelCleared);  // Draw level information

        delay(20);  // Adjust delay for game speed
    }

    getch();
    closegraph();
    return 0;
}
