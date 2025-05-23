#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QSlider>
#include <QPushButton>
#include <QProcess>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayerStateChanged(QMediaPlayer::State state);
    void on_btn_pause_clicked();
    void on_btn_record_clicked();
    void on_btn_stop_clicked();
    void on_btn_save_clicked();
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void on_sld_pos_sliderMoved(int pos);
    void on_sld_volume_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QMediaPlayer* player_ = nullptr;
    QProcess* ffmpeg_process_ = nullptr;
    double recording_start_time_ = -1;
    double recording_end_time_ = -1;
    QLabel* recording_label_ = nullptr;
    QTimer* recording_timer_ = nullptr;
    QPixmap recording_icon_on_;
    QPixmap recording_icon_off_;
};
#endif // MAINWINDOW_H
