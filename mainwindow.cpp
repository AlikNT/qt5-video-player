#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    player_ = new QMediaPlayer(this);
    player_->setVideoOutput(ui->video_output);

    // Индикатор записи с иконкой
    recording_label_ = new QLabel(this);
    recording_icon_on_ = QPixmap(12, 12);
    recording_icon_on_.fill(Qt::red);
    recording_icon_off_ = QPixmap(12, 12);
    recording_icon_off_.fill(Qt::transparent);
    recording_label_->setPixmap(recording_icon_off_);
    recording_label_->setVisible(false);
    ui->statusBar->addPermanentWidget(recording_label_);

    recording_timer_ = new QTimer(this);
    connect(recording_timer_, &QTimer::timeout, this, [this]() {
        static bool visible = false;
        if (recording_label_) {
            recording_label_->setPixmap(visible ? recording_icon_on_ : recording_icon_off_);
            visible = !visible;
        }
    });

    connect(player_, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(player_, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);

    connect(ui->sld_pos, &QSlider::sliderMoved, this, &MainWindow::on_sld_pos_sliderMoved);
    connect(ui->sld_volume, &QSlider::valueChanged, this, &MainWindow::on_sld_volume_valueChanged);

    connect(ui->btn_choose, &QPushButton::clicked, this, [this]() {
        QString filename = QFileDialog::getOpenFileName(this, "Open Video", "", "Video Files (*.mp4 *.avi *.mkv)");
        if (!filename.isEmpty()) {
            player_->setMedia(QUrl::fromLocalFile(filename));
            player_->play();
        }
    });
    connect(player_, &QMediaPlayer::stateChanged, this, &MainWindow::onPlayerStateChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (ffmpeg_process_) {
        ffmpeg_process_->kill();
        delete ffmpeg_process_;
    }
}

void MainWindow::onPlayerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState) {
        ui->btn_pause->setText("⏸");  // пауза
        ui->btn_pause->setToolTip("Пауза");
    } else {
        ui->btn_pause->setText("▶");  // воспроизведение
        ui->btn_pause->setToolTip("Воспроизвести");
    }
}


void MainWindow::on_btn_pause_clicked()
{
    if (player_->state() == QMediaPlayer::PlayingState) {
        player_->pause();
    }
    else {
        player_->play();
    }
}

void MainWindow::on_btn_record_clicked()
{
    recording_start_time_ = player_->position() / 1000.0;  // seconds
    ui->statusBar->showMessage("Запись начата. Ожидание остановки...");
    if (recording_label_) {
        recording_label_->setVisible(true);
        recording_label_->setPixmap(recording_icon_on_);
    }

    if (recording_timer_ && !recording_timer_->isActive()) {
        recording_timer_->start(500);
    }
    ui->btn_stop->setEnabled(true);
}

void MainWindow::on_btn_stop_clicked()
{
    recording_end_time_ = player_->position() / 1000.0;  // seconds
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage("Запись остановлена. Готово к сохранению.", 3000);
    if (recording_label_) {
        recording_label_->setVisible(false);
        recording_timer_->stop();
        recording_label_->setPixmap(recording_icon_off_);
    }
    ui->btn_save->setEnabled(true);
}

void MainWindow::on_btn_save_clicked()
{
    QString input = player_->media().request().url().toLocalFile();
    if (input.isEmpty() || recording_start_time_ < 0 || recording_end_time_ <= recording_start_time_) {
        ui->statusBar->showMessage("Неверные границы записи", 3000);
        return;
    }

    QString final_path = QFileDialog::getSaveFileName(this, "Сохранить запись", "output.mp4", "MP4 Video (*.mp4)");
    if (final_path.isEmpty()) return;

    double duration = recording_end_time_ - recording_start_time_;
    ui->statusBar->showMessage("Сохраняем запись...");

    QtConcurrent::run([=]() {
        QProcess ffmpeg;
        ffmpeg.start("ffmpeg", QStringList()
                                   << "-y"
                                   << "-ss" << QString::number(recording_start_time_, 'f', 3)
                                   << "-i" << input
                                   << "-t" << QString::number(duration, 'f', 3)
                                   << "-c" << "copy"
                                   << final_path);
        ffmpeg.waitForFinished(-1);

        QMetaObject::invokeMethod(this, [this]() {
            ui->statusBar->showMessage("Сохранение завершено", 3000);
        }, Qt::QueuedConnection);
    });
    ui->btn_stop->setDisabled(true);
    ui->btn_save->setDisabled(true);
}

void MainWindow::onPositionChanged(qint64 pos)
{
    ui->sld_pos->setValue(static_cast<int>(pos));
}

void MainWindow::onDurationChanged(qint64 dur)
{
    ui->sld_pos->setMaximum(static_cast<int>(dur));
}

void MainWindow::on_sld_pos_sliderMoved(int pos)
{
    player_->setPosition(pos);
}

void MainWindow::on_sld_volume_valueChanged(int value)
{
    player_->setVolume(value);
}


