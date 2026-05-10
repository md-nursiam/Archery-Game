#include "Arrow.h"
#include "Game.h"
#include <QGraphicsScene>
#include <QList>
#include <QTimer>
#include <qmath.h> // qAbs এর জন্য

extern Game *game; // মেইন গেম অবজেক্টকে চেনার জন্য

Arrow::Arrow(QGraphicsItem *parent) : QObject(), QGraphicsRectItem(parent) {
    // ১. তীরের ডিজাইন (লম্বা লাঠির মতো)
    setRect(0, 0, 60, 4);
    setBrush(Qt::black); // তীরের রং কালো

    dy = 0; // শুরুতে সোজা যাবে
}

void Arrow::shoot() {
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(move()));
    timer->start(15); // প্রতি ১৫ মিলিসেকেন্ডে আপডেট হবে
}

void Arrow::move() {
    // ২. তীরের ফিজিক্স (একটু বাঁকা হয়ে নিচে নামবে - প্যারাবোলিক গতি)
    dy += 0.1; // গ্রাভিটি
    setPos(x() + 10, y() + dy); // X-এ ১০ করে আগাবে, Y-এ গ্রাভিটি বাড়বে
    setRotation(dy * 2); // তীর নিচের দিকে মুখ করবে

    // ৩. কলিশন ডিটেকশন (নিশানায় লাগল কিনা)
    QList<QGraphicsItem *> colliding_items = collidingItems();
    for (int i = 0, n = colliding_items.size(); i < n; ++i) {
        // যদি টার্গেটের সাথে ধাক্কা লাগে (Target আমরা EllipseItem দিয়ে বানাবো)
        if (typeid(*(colliding_items[i])) == typeid(QGraphicsEllipseItem)) {

            // স্কোরিং লজিক (তীর সেন্টারের কত কাছে লেগেছে)
            // টার্গেটের সেন্টার পয়েন্ট বের করা
            double targetCenterY = colliding_items[i]->y() + 50; // 50 হলো রেডিয়াস
            double arrowY = this->y();
            double diff = qAbs(targetCenterY - arrowY);

            // দূরত্ব অনুযায়ী পয়েন্ট
            if(diff < 15) game->increaseScore(100); // বুলস আই (মাঝখানে)
            else if(diff < 30) game->increaseScore(50); // মাঝামাঝি
            else game->increaseScore(20); // সাইডে

            // তীর মুছে ফেলা
            scene()->removeItem(this);
            delete this;
            return;
        }
    }

    // ৪. যদি স্ক্রিনের বাইরে চলে যায়
    if (x() > 1000 || y() > 800) {
        scene()->removeItem(this);
        delete this;
    }
}
