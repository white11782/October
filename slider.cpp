#include "slider.h"

slider::slider()
{

}

void slider::getRange(qint64 playtime,QSlider *slider)
{
    slider->setMaximum(playtime);
}
