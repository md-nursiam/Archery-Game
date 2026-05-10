#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <QDebug>
#include <QRandomGenerator>
#include <QDir>
#include <QUrl>
#include <QFont>

#define PI 3.14159265

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(850, 600);


    shootSound = new QSoundEffect(this);
    shootSound->setSource(QUrl::fromLocalFile("shoot.wav"));
    shootSound->setVolume(0.5f);

    hitSound = new QSoundEffect(this);
    hitSound->setSource(QUrl::fromLocalFile("hit.wav"));
    hitSound->setVolume(1.0f);

    winSound = new QSoundEffect(this);
    winSound->setSource(QUrl::fromLocalFile("win.wav"));
    winSound->setVolume(1.0f);

    loseSound = new QSoundEffect(this);
    loseSound->setSource(QUrl::fromLocalFile("lose.wav"));
    loseSound->setVolume(1.0f);


    arrow = nullptr;
    bow = nullptr;
    targetPole = nullptr;
    focusPoint = nullptr;
    messageText = nullptr;
    gameTimer = nullptr;
    countdownTimer = nullptr;

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGameLoop);

    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::updateCountdown);

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 500);

    view = new QGraphicsView(scene, this);
    view->setGeometry(10, 50, 810, 510);


    QPixmap bg("bg.png");
    if (!bg.isNull()) {
        scene->setBackgroundBrush(bg.scaled(800, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        scene->setBackgroundBrush(QColor(135, 206, 235));
    }


    levelLabel = new QLabel("Level: 1", this);
    levelLabel->setGeometry(20, 10, 100, 30);
    levelLabel->setStyleSheet("font-size: 16px; font-weight: bold; background: white; padding: 5px; border: 1px solid black;");


    scoreLabel = new QLabel("Score: 0", this);
    scoreLabel->setGeometry(130, 10, 150, 30);
    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; background: white; padding: 5px; border: 1px solid black;");


    targetScore = 50;
    targetLabel = new QLabel("Target: " + QString::number(targetScore), this);
    targetLabel->setGeometry(350, 10, 150, 30);
    targetLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: blue; background: white; padding: 5px; border: 1px solid black;");


    timeLeft = 30;
    timeLabel = new QLabel("Time: " + QString::number(timeLeft), this);
    timeLabel->setGeometry(700, 10, 100, 30);
    timeLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: red; background: white; padding: 5px; border: 1px solid black;");


    score = 0;
    currentLevel = 1;
    isFired = false;
    isGameOver = false;
    isLevelTransition = false;

    setupScene();
    countdownTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupScene()
{
    QPixmap bowImg("bow.png");
    if(bowImg.isNull()) {
        QGraphicsRectItem *dummyBow = new QGraphicsRectItem(50, 250, 20, 100);
        dummyBow->setBrush(Qt::darkRed);
        scene->addItem(dummyBow);
    } else {
        bow = new QGraphicsPixmapItem(bowImg.scaled(100, 100, Qt::KeepAspectRatio));
        bow->setPos(20, 240);
        bow->setZValue(0);
        scene->addItem(bow);
    }

    createTarget();
    resetArrow();
}

void MainWindow::createTarget()
{
    targetX = 750;
    targetPole = new QGraphicsLineItem(targetX, 50, targetX, 450);
    QPen polePen(Qt::black);
    polePen.setWidth(4);
    targetPole->setPen(polePen);
    scene->addItem(targetPole);

    targetFocusY = 250;
    focusPoint = new QGraphicsEllipseItem(targetX - 10, targetFocusY - 10, 20, 20);
    focusPoint->setBrush(Qt::red);
    focusPoint->setPen(Qt::NoPen);
    scene->addItem(focusPoint);
}

void MainWindow::moveFocusPoint()
{
    int randomY = QRandomGenerator::global()->bounded(50, 450);
    targetFocusY = randomY;
    focusPoint->setRect(targetX - 10, targetFocusY - 10, 20, 20);
}

void MainWindow::resetArrow()
{
    if (isGameOver || isLevelTransition) return;

    if(arrow != nullptr) {
        scene->removeItem(arrow);
        delete arrow;
        arrow = nullptr;
    }

    arrowY = 290;
    arrowX = 50;

    arrow = new QGraphicsLineItem(0, 0, 60, 0);
    QPen pen(Qt::black);
    pen.setWidth(2);
    arrow->setPen(pen);

    arrow->setPos(arrowX, arrowY);
    arrow->setZValue(1);
    arrow->setRotation(0);
    scene->addItem(arrow);

    isFired = false;

    moveFocusPoint();
    if(gameTimer) gameTimer->stop();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (isGameOver || isLevelTransition) return;

    if (!isFired && timeLeft > 0) {
        double mouseX = event->pos().x() - 10;
        double mouseY = event->pos().y() - 50;

        double dx = mouseX - arrowX;
        double dy = mouseY - arrowY;
        double angle = atan2(dy, dx);

        arrow->setRotation(angle * 180 / PI);

        double speed = 30.0;
        velocityX = speed * cos(angle);
        velocityY = speed * sin(angle);

        shootSound->play();

        isFired = true;
        gameTimer->start(20);
    }
}

void MainWindow::updateGameLoop()
{
    if (!isFired || arrow == nullptr) return;

    arrowX += velocityX;
    arrowY += velocityY;
    velocityY += 0.3;

    arrow->setPos(arrowX, arrowY);

    double angle = atan2(velocityY, velocityX) * 180 / PI;
    arrow->setRotation(angle);

    if (arrowX > 800 || arrowY > 500 || arrowY < 0) {
        resetArrow();
    }

    checkCollision();
}

void MainWindow::checkCollision()
{
    if(arrow == nullptr) return;

    double tipX = arrowX + 60;
    double tipY = arrowY;

    if (tipX >= targetX && tipX <= targetX + 20) {
        double distance = std::abs(tipY - targetFocusY);

        int points = 0;

        if (distance <= 15) {
            points = 10;
            hitSound->play();
        } else if (distance <= 40) {
            points = 4;
        } else if (distance <= 150) {
            points = 2;
        }

        if (points > 0) {
            score += points;
            scoreLabel->setText("Score: " + QString::number(score));


            if (score >= targetScore) {

                nextLevel();
            } else {
                resetArrow();
            }
        }
    }
}

void MainWindow::updateCountdown()
{
    if (isGameOver || isLevelTransition) return;

    timeLeft--;
    timeLabel->setText("Time: " + QString::number(timeLeft));

    if (timeLeft <= 0) {
        gameOver();
    }
}


void MainWindow::nextLevel()
{
    isLevelTransition = true;
    gameTimer->stop();
    countdownTimer->stop();


    winSound->play();


    if(messageText != nullptr) {
        scene->removeItem(messageText);
        delete messageText;
    }

    messageText = new QGraphicsTextItem("LEVEL UP!");
    messageText->setDefaultTextColor(Qt::blue);
    QFont font("Arial", 60, QFont::Bold);
    messageText->setFont(font);

    double textWidth = messageText->boundingRect().width();
    double textHeight = messageText->boundingRect().height();
    messageText->setPos((800 - textWidth) / 2, (500 - textHeight) / 2);
    messageText->setZValue(10);
    scene->addItem(messageText);


    QTimer::singleShot(2000, this, &MainWindow::startNextLevel);
}

void MainWindow::startNextLevel()
{

    if(messageText != nullptr) {
        scene->removeItem(messageText);
        delete messageText;
        messageText = nullptr;
    }


    currentLevel++;
    levelLabel->setText("Level: " + QString::number(currentLevel));


    targetScore += 30;
    score = 0;
    timeLeft = 30 + (currentLevel * 5);

    scoreLabel->setText("Score: 0");
    targetLabel->setText("Target: " + QString::number(targetScore));
    timeLabel->setText("Time: " + QString::number(timeLeft));


    isLevelTransition = false;
    resetArrow();
    countdownTimer->start(1000);
}

void MainWindow::gameOver()
{
    if (isGameOver) return;
    isGameOver = true;

    countdownTimer->stop();
    gameTimer->stop();

    if(messageText != nullptr) {
        scene->removeItem(messageText);
        delete messageText;
    }

    messageText = new QGraphicsTextItem("GAME OVER");
    messageText->setDefaultTextColor(Qt::red);
    loseSound->play();

    QFont font("Arial", 60, QFont::Bold);
    messageText->setFont(font);

    double textWidth = messageText->boundingRect().width();
    double textHeight = messageText->boundingRect().height();
    messageText->setPos((800 - textWidth) / 2, (500 - textHeight) / 2);
    messageText->setZValue(10);
    scene->addItem(messageText);
}
