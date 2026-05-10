#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QLabel>
#include <QMouseEvent>
#include <QSoundEffect>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void updateGameLoop();
    void updateCountdown();
    void startNextLevel();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;

    QGraphicsPixmapItem *bow;
    QGraphicsLineItem *arrow;
    QGraphicsLineItem *targetPole;
    QGraphicsEllipseItem *focusPoint;
    QGraphicsTextItem *messageText;

    QTimer *gameTimer;
    QTimer *countdownTimer;

    QSoundEffect *shootSound;
    QSoundEffect *hitSound;
    QSoundEffect *winSound;
    QSoundEffect *loseSound;

    double arrowX, arrowY;
    double velocityX, velocityY;

    bool isFired;
    bool isGameOver;
    bool isLevelTransition;

    int score;
    int timeLeft;
    int targetScore;
    int currentLevel;

    double targetX;
    double targetFocusY;

    QLabel *scoreLabel;
    QLabel *timeLabel;
    QLabel *targetLabel;
    QLabel *levelLabel;

    void setupScene();
    void createTarget();
    void moveFocusPoint();
    void resetArrow();
    void checkCollision();
    void nextLevel();
    void gameOver();
};

#endif // MAINWINDOW_H
