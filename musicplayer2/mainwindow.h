#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QListWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class MusicPlayer : public QMainWindow {
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QListWidget *songListWidget;
    QList<QUrl> songList;
    QStringList songNames;

    void loadSong();
    void Playlist();
};

#endif // MAINWINDOW_H
