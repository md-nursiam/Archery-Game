#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QTimer>

class Arrow : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    Arrow(QGraphicsItem *parent = 0);
    void shoot(); // তীর ছোঁড়ার ফাংশন

public slots:
    void move(); // তীরের এনিমেশন লুপ

private:
    QTimer *timer;
    double dy; // গ্রাভিটি এফেক্টের জন্য (Y অক্ষের পরিবর্তন)
};

#endif // ARROW_H
