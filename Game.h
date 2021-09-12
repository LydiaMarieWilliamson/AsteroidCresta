#ifndef OnceOnlyGame_h
#define OnceOnlyGame_h

// Asteroid Style Game: The gaming visible area.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <time.h>
#include <QWidget>
#include <QColor>
#include "Engine.h"

class QTimer;
class QPainter;

namespace Phonon {
class MediaObject;
} // end of namespace Phonon

class Game: public QWidget {
Q_OBJECT
public:
// The game state.
   typedef int GameStateType;
   static const GameStateType GS_INTRO_0 = 0, GS_INTRO_1 = 1, GS_INTRO_2 = 2, GS_PLAY = 3, GS_DEMO = 4;
private:
   bool m_paused, m_sounds, m_music, m_playing;
   bool m_soundKeydown, m_musicKeydown, m_pauseKeydown;
   time_t m_timeMark;
   double m_defaultGameArea;
   GameStateType m_gameState;
   QColor m_backCol, m_foreCol;
   QTimer *_Timer;
   asteroid::Engine *mp_engine;
// Sound players
   Phonon::MediaObject *mp_musicAudio, *mp_explodeAudio, *mp_thrustAudio, *mp_fireAudio, *mp_eventAudio;
   int m_txs() const;
   double m_scale() const;
   void m_recalcGameArea();
   void m_setFont(QPainter &p, asteroid::LFSize sz, bool bold = false);
   int m_textOut(QPainter &p, const QString &s, int x, int y, Qt::Alignment layout = Qt::AlignLeft | Qt::AlignTop);
   void m_resetScreen(QPainter &p);
   void m_drawPlay();
   void m_drawIntroScreen0();
   void m_drawIntroScreen1();
   void m_drawIntroScreen2();
private slots:
   void m_poll();
protected:
   virtual void paintEvent(QPaintEvent *event);
public:
   Game(QWidget *parent = nullptr);
   ~Game();
   void pause(bool p);
   bool isPaused() const;
   void play(bool p);
   bool isPlaying() const;
   Game::GameStateType gameState() const;
   void setGameState(Game::GameStateType gs);
   int hiscore() const;
   void setHiscore(int hs);
   bool sounds() const;
   void setSounds(bool s);
   bool music() const;
   void setMusic(bool m);
   QColor foreground() const;
   void setForeground(const QColor &c);
   QColor background() const;
   void setBackground(const QColor &c);
   double difficulty() const;
   void setDifficulty(const double &dif);
   int pollRate() const;
   void setPollRate(int ms);
   bool keyDown(int k);
   bool keyUp(int k);
};

#endif // OnceOnly
