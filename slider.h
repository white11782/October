#include <QMediaPlayer>
#include <QSlider>

#ifndef SLIDER_H
#define SLIDER_H


class slider
{
public:
    slider();
    void getRange(qint64 playtime,QSlider *slider);
private:
    float range;

};

#endif // SLIDER_H
