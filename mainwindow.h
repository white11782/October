#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QFile>
#include <QPushButton>
#include <QSplitter>
#include <QListWidget>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QKeyEvent>
#include <QTimer>
#include <slider.h>
#include <pushbutton.h>
#include <QLabel>
#include <QListWidgetItem>
#include <QScreen>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void openFile();
    void setVolume();
private slots:
    void on_actiondakaiwenjian_triggered();

    void displayVideo();

    void stopDisplay();

    void on_actionquanping_triggered();

    void setCursors();

    void hide_show_playList();

    void setSlider();


    void showTime();

    void changePosition();

    void changePositionEnd();

    void changeVolume();

    void changeVolumeEnd();

    void mute();

    void replay();

    void playNext();

    void playPrevious();
    void on_actionjietu_triggered();

protected:
    void keyPressEvent(QKeyEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QPushButton *btn_play = new QPushButton();
    QPushButton *btn_volume = new QPushButton();
    QPushButton *btn_fullScreen = new QPushButton();
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    QString fileName;

    QWidget *widget_displayList = new QWidget();
    QSplitter *splitter_video = new QSplitter();

    QVideoWidget *videoWidget = new QVideoWidget();
    QWidget *widget_control = new QWidget();

    QListWidget *playListWidget = new QListWidget(widget_displayList);
    QListWidget *unVisibleList = new QListWidget(widget_displayList);

    QSlider *slider = new QSlider(widget_control);

    QSlider *audio_slider = new QSlider(widget_control);

    class slider *myslider = new class slider();

    QTimer *timer = new QTimer(this);

    bool isHide = true;
    bool isFullScreen = false;

    QTimer *volume_timer = new QTimer(this);

    QTimer *progress_timer = new QTimer();

    QLabel *labelTime = new QLabel(widget_control);

    QString position_time;
    QString duration_time;

    int secs;
    int mins;
    int hours;
    int positionStart;
    int positionEnd;
    int i = 20;
};
#endif // MAINWINDOW_H
