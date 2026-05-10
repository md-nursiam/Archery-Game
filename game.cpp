#include "Game.h"
#include "Arrow.h"
#include <QFont>
#include <QBrush>
#include <QDebug>

Game::Game(QWidget *parent) {
    // ১. সিন সেটআপ
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 1000, 600);
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(1000, 600);

    currentLevel = 1;
    startLevel();
}

void Game::startLevel() {
    score = 0;
    targetScore = currentLevel * 100; // লেভেল ১ এ ১০০ পয়েন্ট দরকার
    timeLeft = 15; // ১৫ সেকেন্ড সময়

    scene->clear(); // আগের সব মুছে ফেলা

    // UI দেখানো
    uiText = new QGraphicsTextItem();
    uiText->setPlainText(QString("Level: %1  |  Target: %2  |  Score: 0  |  Time: %3")
                             .arg(currentLevel).arg(targetScore).arg(timeLeft));
    uiText->setFont(QFont("Arial", 20));
    uiText->setDefaultTextColor(Qt::blue);
    uiText->setPos(10, 10);
    scene->addItem(uiText);

    // আর্চার তৈরি (সবুজ বক্স) - এখানে আপনি আর্চারের ছবি বসাতে পারেন
    archer = new QGraphicsRectItem(0, 0, 50, 100);
    archer->setPos(50, 400);
    archer->setBrush(Qt::green);
    scene->addItem(archer);

    // টার্গেট তৈরি (লাল গোল্লা)
    // লেভেল বাড়লে টার্গেট দূরে যাবে: 600 + (লেভেল * 100)
    int targetX = 500 + (currentLevel * 80);

    // টার্গেটের ৩টি রিং আঁকছি (লজিক সিম্পল রাখার জন্য মেইন টার্গেট একটাই রাখছি)
    QGraphicsEllipseItem *outerRing = new QGraphicsEllipseItem(0, 0, 100, 100);
    outerRing->setPos(targetX, 400);
    outerRing->setBrush(Qt::white);
    scene->addItem(outerRing);

    QGraphicsEllipseItem *middleRing = new QGraphicsEllipseItem(15, 15, 70, 70);
    middleRing->setPos(targetX, 400); // পজিশন সেম, প্যারেন্ট করলে ভালো হতো, তবে সিম্পল রাখছি
    middleRing->setBrush(Qt::red); // কিন্তু ডিটেকশনের জন্য আমরা শুধু মেইন outerRing কেই টার্গেট ধরব Arrow.cpp তে
    // এখানে আমরা শুধু ভিজ্যুয়াল এফেক্ট দিচ্ছি, মেইন টার্গেট Arrow.cpp তে চেক হচ্ছে

    // টাইমার চালু
    gameTimer = new QTimer(this);
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(updateTimer()));
    gameTimer->start(1000);
}

void Game::mousePressEvent(QMouseEvent *event) {
    // মাউস ক্লিক করলে তীর বের হবে
    Arrow *arrow = new Arrow();
    arrow->setPos(archer->x() + 50, archer->y() + 20); // আর্চারের বুক থেকে তীর বের হবে
    scene->addItem(arrow);
    arrow->shoot();
}

void Game::increaseScore(int amount) {
    score += amount;
    // UI আপডেট
    uiText->setPlainText(QString("Level: %1  |  Target: %2  |  Score: %3  |  Time: %4")
                             .arg(currentLevel).arg(targetScore).arg(score).arg(timeLeft));

    // লেভেল পাশ চেক করা
    if (score >= targetScore) {
        gameTimer->stop();
        if (currentLevel < 5) {
            currentLevel++;
            startLevel(); // পরের লেভেল শুরু
        } else {
            gameOver(true); // সব লেভেল শেষ
        }
    }
}

void Game::updateTimer() {
    timeLeft--;
    uiText->setPlainText(QString("Level: %1  |  Target: %2  |  Score: %3  |  Time: %4")
                             .arg(currentLevel).arg(targetScore).arg(score).arg(timeLeft));

    if (timeLeft <= 0) {
        gameTimer->stop();
        gameOver(false); // সময় শেষ, হেরে গেছেন
    }
}

void Game::gameOver(bool win) {
    scene->clear();
    QGraphicsTextItem *msg = new QGraphicsTextItem();
    if (win) {
        msg->setPlainText("YOU WON THE TOURNAMENT!");
        msg->setDefaultTextColor(Qt::green);
    } else {
        msg->setPlainText("GAME OVER! Try Again.");
        msg->setDefaultTextColor(Qt::red);
    }
    msg->setFont(QFont("Arial", 40, QFont::Bold));
    msg->setPos(200, 250);
    scene->addItem(msg);
}
