#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


//存在的问题：上下分割比例不正确；右侧未加载处视频信息
    this->setCentralWidget(splitter);

    QHBoxLayout *hbLayout = new QHBoxLayout(widget_displayList);
    hbLayout->addWidget(playListWidget);
    hbLayout->setContentsMargins(0,0,0,0);
//设置一不可见的列表用来存储地址
    unVisibleList->setVisible(false);

    splitter->setHandleWidth(1);
    splitter->addWidget(splitter_video);
    splitter->addWidget(widget_control);

    splitter_video->setHandleWidth(1);
    splitter_video->addWidget(videoWidget);
    splitter_video->addWidget(widget_displayList);
    //划分左右的分割比例
    QSizePolicy leftPolicy = videoWidget->sizePolicy();
    leftPolicy.setHorizontalStretch(4);
    videoWidget->setSizePolicy(leftPolicy);
    QSizePolicy rightPolicy = widget_displayList->sizePolicy();
    rightPolicy.setHorizontalStretch(1);
    widget_displayList->setSizePolicy(rightPolicy);

    //已经移除状态栏，现在利用QSplitter划分区域，让播放暂停等按钮显示在下方窗口
    QPushButton *btn_stop = new QPushButton(widget_control);
    btn_stop->setIcon(QIcon(u8":/image/stop.png"));
    btn_stop->setFixedSize(40,40);

    QPushButton *btn_previous = new QPushButton(widget_control);
    btn_previous->setIcon(QIcon(u8":/image/previous.png"));
    btn_previous->setFixedSize(40,40);

    QPushButton *btn_next = new QPushButton(widget_control);
    btn_next->setIcon(QIcon(u8":/image/next.png"));
    btn_next->setFixedSize(40,40);

    btn_volume->setParent(widget_control);
    btn_volume->setIcon(QIcon(u8":/image/volume.png"));
    btn_volume->setFixedSize(40,40);

    btn_fullScreen->setParent(widget_control);
    btn_fullScreen->setIcon(QIcon(":/image/quanping.png"));
    btn_fullScreen->setFixedSize(40,40);
    isFullScreen = false;

    QPushButton *btn_hidePlayList = new QPushButton(widget_control);
    btn_hidePlayList->setText(u8"播放列表");
    btn_hidePlayList->setStyleSheet("color:pink");

    //增加一个全屏的按钮

    btn_play->setParent(widget_control);
    btn_play->setIcon(QIcon(u8":/image/bofang.png"));
    btn_play->setFixedSize(40,40);

    connect(btn_play,SIGNAL(clicked()),this,SLOT(displayVideo()));
    connect(btn_stop,SIGNAL(clicked()),this,SLOT(stopDisplay()));
    connect(btn_hidePlayList,SIGNAL(clicked()),this,SLOT(hide_show_playList()));
    connect(btn_volume,SIGNAL(clicked()),this,SLOT(mute()));
    connect(btn_next,SIGNAL(clicked()),this,SLOT(playNext()));
    connect(btn_previous,SIGNAL(clicked()),this,SLOT(playPrevious()));
    connect(btn_fullScreen,SIGNAL(clicked()),this,SLOT(on_actionquanping_triggered()));

    //设置按键布局
    QHBoxLayout *hbLayoutCtrl = new QHBoxLayout();
    hbLayoutCtrl->addWidget(labelTime);
    hbLayoutCtrl->addStretch();
    hbLayoutCtrl->addStretch();
    hbLayoutCtrl->addStretch();
    hbLayoutCtrl->addWidget(btn_stop);
    hbLayoutCtrl->addWidget(btn_previous);
    hbLayoutCtrl->addWidget(btn_play);
    hbLayoutCtrl->addWidget(btn_next);
    hbLayoutCtrl->addWidget(btn_volume);
    hbLayoutCtrl->addWidget(audio_slider);
    hbLayoutCtrl->addStretch();
    hbLayoutCtrl->addStretch();
    hbLayoutCtrl->addWidget(btn_fullScreen);
    hbLayoutCtrl->addWidget(btn_hidePlayList);

    //进度条的设置 涉及到多个布局混合的方式，一定要在最终的布局里面再设置父窗口指针
    slider->setOrientation(Qt::Horizontal);
    slider->setVisible(false);
    audio_slider->setOrientation(Qt::Horizontal);
    QHBoxLayout *hbSliderLayout = new QHBoxLayout();
    hbSliderLayout->addWidget(slider);

    QVBoxLayout *vbLayout = new QVBoxLayout(widget_control);
    vbLayout->addLayout(hbSliderLayout);
    vbLayout->addLayout(hbLayoutCtrl);
    vbLayout->setMargin(0);

    //上下分割
    QSizePolicy topPolicy = splitter_video->sizePolicy();
    topPolicy.setVerticalStretch(30);
    splitter_video->setSizePolicy(topPolicy);
    QSizePolicy downPolicy = widget_control->sizePolicy();
    downPolicy.setVerticalStretch(1);
    widget_control->setSizePolicy(downPolicy);

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(player);

    videoWidget->setFocusPolicy(Qt::StrongFocus);
    videoWidget->setFocus();

    isHide = false;

    QSplitterHandle *pHandle = splitter->handle(1);
    pHandle->setEnabled(false);
    pHandle->setStyleSheet("background-color:rgba(0,48,77,0.7)");
    QSplitterHandle *cHandle = splitter_video->handle(1);
    cHandle->setEnabled(false);
    cHandle->setStyleSheet("background-color:rgba(0,48,77,0.7)");

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(setSlider()));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(showTime()));
    connect(slider,SIGNAL(sliderMoved(int)),this,SLOT(changePosition()));
    connect(slider,SIGNAL(sliderReleased()),this,SLOT(changePositionEnd()));
    connect(audio_slider,SIGNAL(sliderMoved(int)),this,SLOT(changeVolume()));
    connect(audio_slider,SIGNAL(sliderReleased()),this,SLOT(changeVolumeEnd()));

    connect(playListWidget,SIGNAL(itemSelectionChanged()),this,SLOT(replay()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{
    fileName = QFileDialog::getOpenFileName(this,
                                                    tr(u8"打开"),
                                                    "",
                                                    tr(u8"所有类型(*.*)"));
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {

        /*每个QMainWindow必须要有而且仅有一个主控件，这地方把QSplitter设置为主控件，
        videoWidget和widget都位于QSplitter上，分别在左右，这样就可以实现多个窗口
        */

        player->stop();

        player->setVideoOutput(videoWidget);
        playlist->clear();

        playlist->addMedia(QUrl::fromLocalFile(fileName));

        playlist->setCurrentIndex(0);

        player->setPlaylist(playlist);

        player->play();
        widget_displayList->show();
        isHide = false;
        slider->setVisible(true);
        //设置播放列表
        setVolume();
        QStringList fileSplitter = fileName.split("/");
        playListWidget->addItem(fileSplitter.last());
        unVisibleList->addItem(fileName);
        this->setWindowTitle(fileSplitter.last());
        i++;
    }
    else
    {
        return;
    }
}

void MainWindow::on_actiondakaiwenjian_triggered()
{
    openFile();
}

void MainWindow::displayVideo()
{
    if(player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        openFile();
    }
    else if(player->state() == QMediaPlayer::PlayingState)
    {
        //暂停
        btn_play->setIcon(QIcon(u8":/image/cut.png"));
        player->pause();
    }
    else
    {
        //播放
        btn_play->setIcon(QIcon(u8":/image/bofang.png"));
        player->play();
    }

}

void MainWindow::stopDisplay()
{
    if(player->state() == QMediaPlayer::PlayingState||player->state() == QMediaPlayer::PausedState)
    {
        player->stop();
        playlist->clear();
        videoWidget->setStyleSheet("backround:rgba(0,48,77,0.7)");
    }
    else if(player->mediaStatus() == QMediaPlayer::NoMedia) {
        QMessageBox::information(this,u8"提示",
                                 u8"当前没有视频文件",
                                 QMessageBox::Yes);
    }
}

void MainWindow::on_actionquanping_triggered()
{
    if(player->mediaStatus() == QMediaPlayer::NoMedia)
    {
        QMessageBox::information(this,u8"提示",
                                 u8"当前没有正在播放的视频",
                                 QMessageBox::Yes);
        return;
    }
    else
    {
        if(isFullScreen == true)
        {
            timer->stop();
            this->menuBar()->show();
            this->widget_displayList->show();
            btn_fullScreen->setIcon(QIcon(":/image/quanping.png"));
            isFullScreen = false;
        }
        else
        {
            connect(timer,SIGNAL(timeout()),this,SLOT(setCursors()));
            timer->start(5000);

            //this->setWindowFlags(Qt::FramelessWindowHint);
            this->menuBar()->hide();
            this->widget_displayList->hide();
            this->showMaximized();
            btn_fullScreen->setIcon(QIcon(":/image/suoxiao.png"));
            isFullScreen = true;
        }
    }
}

void MainWindow::setCursors()
{
    if(this->isMaximized()&&
            (player->state() == QMediaPlayer::PlayingState||player->state() == QMediaPlayer::PausedState))
    {
        videoWidget->setCursor(Qt::BlankCursor);
        widget_control->hide();
    }
}

void MainWindow::hide_show_playList()
{
    if(isHide == false)
    {
        widget_displayList->hide();
        isHide = true;
    }
    else {
        widget_displayList->show();
        isHide = false;
    }
}

void MainWindow::setSlider()
{
    slider->setMaximum(player->duration());
    slider->setValue(player->position());
    secs = player->position()/1000;
    mins = secs/60;
    hours = mins/60;
    secs = secs%60;
    position_time = QString::asprintf("%d:%d:%d",hours,mins,secs);
    labelTime->setText("       "+position_time+"/"+duration_time);
}

void MainWindow::setVolume()
{
    audio_slider->setMaximum(player->volume());
    audio_slider->setValue(player->volume());
}

void MainWindow::showTime()
{
    int secs = player->duration()/1000;//全部秒数
    int mins = secs/60;//全部分钟数
    int hours = mins/60;
    secs = secs%60;
    duration_time = QString::asprintf("%d:%d:%d",hours,mins,secs);
}

void MainWindow::changePosition()
{
    player->setPosition(slider->value());
}

void MainWindow::changePositionEnd()
{
    player->setPosition(slider->value());
    positionEnd = player->position();
    slider->setValue(positionEnd);
}

void MainWindow::changeVolume()
{
    player->setVolume(audio_slider->value());
}

void MainWindow::changeVolumeEnd()
{
    player->setVolume(audio_slider->value());
    audio_slider->setValue(player->volume());
}

void MainWindow::mute()
{
    if(player->volume() != 0){
        player->setVolume(0);
        audio_slider->setValue(0);
        btn_volume->setIcon(QIcon(":/image/volume_press.png"));
    }
    else
    {
        player->setVolume(20);
        audio_slider->setValue(player->volume());
        btn_volume->setIcon(QIcon(":/image/volume.png"));
    }
}

void MainWindow::replay()
{
    QListWidgetItem *item = playListWidget->currentItem();
    QList<QListWidgetItem*>fileItems = unVisibleList->findItems(item->text(),Qt::MatchEndsWith);
    player->stop();
    player->setVideoOutput(videoWidget);
    playlist->clear();
    playlist->addMedia(QUrl::fromLocalFile(fileItems.at(0)->text()));

    playlist->setCurrentIndex(0);

    player->setPlaylist(playlist);
    player->play();
    //设置播放列表
    setVolume();
    QStringList fileSplitter = fileItems.at(0)->text().split("/");
    this->setWindowTitle(fileSplitter.last());
}
//快进
void MainWindow::playNext()
{
    if(player->duration()>=(player->position()+1000))
    {
        player->setPosition(player->position()+1000);
        slider->setValue(player->position());
    }
}
//快退
void MainWindow::playPrevious()
{
    if(player->position()-1000>=0)
    {
        player->setPosition(player->position()-1000);
        slider->setValue(player->position());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()== Qt::Key_Escape)
    {
        timer->stop();
        this->menuBar()->show();
        this->widget_displayList->show();
        btn_fullScreen->setIcon(QIcon(":/image/quanping.png"));
        isFullScreen = false;
    }
    if(event->key() == Qt::Key_Space)
    {
        on_actionquanping_triggered();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    videoWidget->setCursor(Qt::ArrowCursor);
    widget_control->show();
}

void MainWindow::on_actionjietu_triggered()
{
    QScreen *screen = QApplication::primaryScreen();
    QPixmap pixmap=screen->grabWindow(videoWidget->winId());
    QApplication::clipboard()->setPixmap(pixmap);
    QMessageBox::information(this,u8"提示",
                             u8"截图已经保存到剪贴板，请粘贴查看",
                             QMessageBox::Yes);
}
