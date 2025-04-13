#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QListWidget>
#include <exception>
#include <string>
#include <vector>
#include <algorithm>

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

// Custom exception class for music player errors
class MusicPlayerException : public std::exception {
private:
    std::string message;

public:
    MusicPlayerException(const std::string& msg) : message(msg) {}
    
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

// Template class for managing playlists of different media types
template <typename MediaItem, typename DisplayInfo>
class PlaylistManager {
private:
    std::vector<MediaItem> items;
    std::vector<DisplayInfo> displayItems;

public:
    PlaylistManager() {}
    
    // Add an item if it doesn't already exist
    bool addItem(const MediaItem& item, const DisplayInfo& display) {
        // Check if item already exists
        auto it = std::find(items.begin(), items.end(), item);
        if (it == items.end()) {
            items.push_back(item);
            displayItems.push_back(display);
            return true;
        }
        return false;
    }
    
    // Get item at specified index
    MediaItem getItem(size_t index) const {
        if (index < items.size()) {
            return items[index];
        }
        throw MusicPlayerException("Playlist index out of bounds");
    }
    
    // Get display info at specified index
    DisplayInfo getDisplayInfo(size_t index) const {
        if (index < displayItems.size()) {
            return displayItems[index];
        }
        throw MusicPlayerException("Playlist index out of bounds");
    }
    
    // Get number of items
    size_t size() const {
        return items.size();
    }
    
    // Check if playlist is empty
    bool isEmpty() const {
        return items.empty();
    }
    
    // Get all display items for UI listing
    std::vector<DisplayInfo> getAllDisplayItems() const {
        return displayItems;
    }
    
    // Find index of item
    int findItem(const MediaItem& item) const {
        auto it = std::find(items.begin(), items.end(), item);
        if (it != items.end()) {
            return static_cast<int>(std::distance(items.begin(), it));
        }
        return -1;
    }
    
    // Remove item at index
    bool removeAt(size_t index) {
        if (index < items.size()) {
            items.erase(items.begin() + index);
            displayItems.erase(displayItems.begin() + index);
            return true;
        }
        return false;
    }
};

// Abstract Player interface - defines pure virtual functions that any player must implement
class IPlayer {
public:
    virtual ~IPlayer() {}
    
    // Pure virtual functions (abstract methods)
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void setSource(const QUrl& source) = 0;
    virtual bool isPlaying() const = 0;
};

// Abstract UI interface - defines the UI operations
class IPlayerUI {
public:
    virtual ~IPlayerUI() {}
    
    // Pure virtual functions for UI operations
    virtual void createControls() = 0;
    virtual void showPlaylist() = 0;
    virtual void updateDisplay(const QString& info) = 0;
    virtual void handleError(const QString& error) = 0;
};

// Multiple inheritance: MusicPlayer inherits from QMainWindow and both interfaces
class MusicPlayer : public QMainWindow, public IPlayer, public IPlayerUI {
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    virtual ~MusicPlayer() {
        try {
            // Clean up resources if needed
        } catch (...) {
            // Catch any exceptions in destructor to prevent undefined behavior
        }
    }
    
    // Implement IPlayer interface methods
    void play() override;
    void pause() override;
    void stop() override;
    void setSource(const QUrl& source) override;
    bool isPlaying() const override;
    
    // Implement IPlayerUI interface methods
    void createControls() override;
    void showPlaylist() override;
    void updateDisplay(const QString& info) override;
    void handleError(const QString& error) override;

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QListWidget *songListWidget;
    QPushButton *loadButton;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *playlistButton;
    
    // Use our template class for playlist management
    PlaylistManager<QUrl, QString> playlist;
    
    // Current song index
    int currentSongIndex;

    void loadSong();
};

#endif // MAINWINDOW_H
