#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QTimer>

class Game : public QGraphicsView {
    Q_OBJECT
public:
    Game(QWidget *parent = 0);

    QGraphicsScene *scene;
    int score;
    int currentLevel;
    int timeLeft;
    int targetScore;

    void startLevel();        // লেভেল শুরু
    void increaseScore(int amount); // স্কোর বাড়ানো
    void gameOver(bool win);  // গেম ওভার বা উইন মেসেজ

    // UI Elements
    QGraphicsTextItem *uiText; // স্কোর ও টাইম দেখানোর জন্য
    QGraphicsRectItem *archer; // আমাদের তীরন্দাজ
    QGraphicsEllipseItem *target; // নিশানা

public slots:
    void updateTimer(); // সময়ের কাটা কমানো

protected:
    void mousePressEvent(QMouseEvent *event); // মাউস ক্লিকে তীর ছোঁড়া

private:
    QTimer *gameTimer;
};

#endif // GAME_H
