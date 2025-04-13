#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QUrl>
#include <QDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <exception>

// Constructor - now using the interface methods
MusicPlayer::MusicPlayer(QWidget *parent) : QMainWindow(parent) {
    try {
        // Initialize current song index
        currentSongIndex = -1;
        
        // Create media player with audio output
        player = new QMediaPlayer(this);
        if (!player) {
            throw MusicPlayerException("Failed to create media player");
        }
        
        audioOutput = new QAudioOutput(this);
        if (!audioOutput) {
            throw MusicPlayerException("Failed to create audio output");
        }
        
        player->setAudioOutput(audioOutput);
        audioOutput->setVolume(0.7); // 70% volume
        
        // Create UI elements through the interface method
        createControls();
    } catch (const MusicPlayerException& e) {
        handleError("Music Player Error: " + QString(e.what()));
    } catch (const std::exception& e) {
        handleError("Initialization Error: " + QString(e.what()));
    }
}

// Implementation of IPlayer interface methods
void MusicPlayer::play() {
    try {
        player->play();
        updateDisplay("Playing: " + (currentSongIndex >= 0 ? 
                     playlist.getDisplayInfo(currentSongIndex) : "No song selected"));
    } catch (const std::exception& e) {
        handleError("Play error: " + QString(e.what()));
    }
}

void MusicPlayer::pause() {
    player->pause();
    updateDisplay("Paused");
}

void MusicPlayer::stop() {
    player->stop();
    updateDisplay("Stopped");
}

void MusicPlayer::setSource(const QUrl& source) {
    try {
        player->setSource(source);
        // Find the index of this song in the playlist
        currentSongIndex = playlist.findItem(source);
        updateDisplay("Ready to play: " + playlist.getDisplayInfo(currentSongIndex));
    } catch (const std::exception& e) {
        handleError("Error setting source: " + QString(e.what()));
    }
}

bool MusicPlayer::isPlaying() const {
    return player->playbackState() == QMediaPlayer::PlayingState;
}

// Implementation of IPlayerUI interface methods
void MusicPlayer::createControls() {
    // Create main window layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    // Create buttons
    loadButton = new QPushButton("Load Music");
    playButton = new QPushButton("Play");
    pauseButton = new QPushButton("Pause");
    playlistButton = new QPushButton("Song Playlist");
    
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
    connect(playButton, &QPushButton::clicked, this, &MusicPlayer::play);
    connect(pauseButton, &QPushButton::clicked, this, &MusicPlayer::pause);
    connect(playlistButton, &QPushButton::clicked, this, &MusicPlayer::showPlaylist);
}

void MusicPlayer::showPlaylist() {
    try {
        // Create a simple dialog
        QDialog dialog(this);
        dialog.setWindowTitle("Song Playlist");
        dialog.resize(400, 300);
        
        // Create layout
        QVBoxLayout* layout = new QVBoxLayout(&dialog);
        
        // Create list widget
        QListWidget* list = new QListWidget(&dialog);
        layout->addWidget(list);
        
        // Get all song names from template
        if (!playlist.isEmpty()) {
            // Use template to get display items
            std::vector<QString> displayItems = playlist.getAllDisplayItems();
            for (const QString& name : displayItems) {
                list->addItem(name);
            }
            
            // Set current selection
            if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(playlist.size())) {
                list->setCurrentRow(currentSongIndex);
            }
        }
        
        // Add Play button
        QPushButton* playButton = new QPushButton("Play Selected", &dialog);
        layout->addWidget(playButton);
        
        // Disable play button if no songs
        if (playlist.isEmpty()) {
            list->addItem("No songs added yet");
            playButton->setEnabled(false);
        }
        
        // Connect play button - with exception handling
        connect(playButton, &QPushButton::clicked, &dialog, [this, list, &dialog]() {
            try {
                int row = list->currentRow();
                if (row >= 0 && row < static_cast<int>(playlist.size())) {
                    // Use template to get the media item
                    QUrl mediaUrl = playlist.getItem(row);
                    setSource(mediaUrl);
                    play();
                    dialog.accept();
                }
            } catch (const MusicPlayerException& e) {
                handleError("Playback Error: " + QString(e.what()));
            } catch (const std::exception& e) {
                handleError("Error playing selected song: " + QString(e.what()));
            }
        });
        
        // Show dialog
        dialog.exec();
    } catch (const std::exception& e) {
        handleError("Playlist Error: " + QString(e.what()));
    }
}

void MusicPlayer::updateDisplay(const QString& info) {
    statusBar()->showMessage(info);
}

void MusicPlayer::handleError(const QString& error) {
    QMessageBox::critical(this, "Error", error);
    updateDisplay("Error: " + error);
}

void MusicPlayer::loadSong() {
    try {
        // Open file dialog to select music files
        QString file = QFileDialog::getOpenFileName(this, 
            "Select Music File", 
            "", 
            "Audio (*.mp3 *.wav *.mp4 *.m4a)");
        
        // If a file was selected
        if (!file.isEmpty()) {
            // Create URL from file path
            QUrl url = QUrl::fromLocalFile(file);
            
            try {
                // Check file validity
                QFileInfo fileInfo(file);
                if (!fileInfo.exists() || !fileInfo.isReadable()) {
                    throw MusicPlayerException("File is not accessible or doesn't exist");
                }
                
                // Get filename for display
                QString name = fileInfo.fileName();
                
                // Add to playlist using template method
                if (playlist.addItem(url, name)) {
                    // If new song added, update current index
                    currentSongIndex = playlist.findItem(url);
                }
                
                // Set as current source
                setSource(url);
                
                updateDisplay("Loaded: " + name);
                
            } catch (const MusicPlayerException& e) {
                handleError("Music File Error: " + QString(e.what()));
            } catch (const std::exception& e) {
                handleError("Media Error: " + QString(e.what()));
            }
        }
    } catch (const std::exception& e) {
        handleError("File Error: " + QString(e.what()));
    }
}
