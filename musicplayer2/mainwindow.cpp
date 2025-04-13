#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QUrl>
#include <QDialog>
#include <QFileInfo>

MusicPlayer::MusicPlayer(QWidget *parent) : QMainWindow(parent) {
    // Create media player with audio output
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.7); // 70% volume
    
    // Create main window layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    // Create buttons
    QPushButton *loadButton = new QPushButton("Load Music");
    QPushButton *playButton = new QPushButton("Play");
    QPushButton *pauseButton = new QPushButton("Pause");
    QPushButton *playlistButton = new QPushButton("Song Playlist");
    
    // Add buttons to layout
    layout->addWidget(loadButton);
    layout->addWidget(playButton);
    layout->addWidget(pauseButton);
    layout->addWidget(playlistButton);
    
    // Setup main window
    setCentralWidget(central);
    setWindowTitle("Music Player");
    resize(300, 200);
    
    // Connect button signals to functions
    connect(loadButton, &QPushButton::clicked, this, &MusicPlayer::loadSong);
    connect(playButton, &QPushButton::clicked, player, &QMediaPlayer::play);
    connect(pauseButton, &QPushButton::clicked, player, &QMediaPlayer::pause);
    connect(playlistButton, &QPushButton::clicked, this, &MusicPlayer::Playlist);
}

void MusicPlayer::loadSong() {
    // Open file dialog to select music files
    QString file = QFileDialog::getOpenFileName(this, 
        "Select Music File", 
        "", 
        "Audio (*.mp3 *.wav *.mp4 *.m4a)");
    
    // If a file was selected
    if (!file.isEmpty()) {
        // Create URL from file path
        QUrl url = QUrl::fromLocalFile(file);
        
        // Set the media source
        player->setSource(url);
        
        // Add to song list if not already there
        if (!songList.contains(url)) {
            // Get just the filename without the path
            QFileInfo fileInfo(file);
            QString name = fileInfo.fileName();
            
            // Add to our lists
            songList.append(url);
            songNames.append(name);
        }
    }
}

void MusicPlayer::Playlist() {
    // Create a simple dialog
    QDialog dialog(this);
    dialog.setWindowTitle("Song Playlist");
    dialog.resize(400, 300);
    
    // Create layout
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Create list widget
    QListWidget* list = new QListWidget(&dialog);
    layout->addWidget(list);
    
    // Add songs to list
    for (int i = 0; i < songNames.size(); i++) {
        list->addItem(songNames[i]);
    }
    
    // Add Play button
    QPushButton* playButton = new QPushButton("Play Selected", &dialog);
    layout->addWidget(playButton);
    
    // Disable play button if no songs
    if (songNames.isEmpty()) {
        list->addItem("No songs added yet");
        playButton->setEnabled(false);
    }
    
    // Connect play button - simple function
    connect(playButton, &QPushButton::clicked, &dialog, [this, list, &dialog]() {
        int row = list->currentRow();
        if (row >= 0 && row < songList.size()) {
            player->setSource(songList[row]);
            player->play();
            dialog.accept();
        }
    });
    
    // Show dialog
    dialog.exec();
}
