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

class GameWidget : public QWidget
{
  Q_OBJECT
public:
  // Game state type
  typedef int GameStateType;
  static const GameStateType GS_INTRO_0 = 0;
  static const GameStateType GS_INTRO_1 = 1;
  static const GameStateType GS_INTRO_2 = 2;
  static const GameStateType GS_PLAY =    3;
  static const GameStateType GS_DEMO =    4;
private:
  // Private members
  bool m_paused;
  bool m_sounds;
  bool m_music;
  bool m_playing;
  bool m_soundKeydown;
  bool m_musicKeydown;
  bool m_pauseKeydown;
  time_t m_timeMark;
  double m_defaultGameArea;
  GameStateType m_gameState;
  QColor m_backCol;
  QColor m_foreCol;
  QTimer* mp_timer;
  asteroid::Engine* mp_engine;

  // Sound players
  Phonon::MediaObject* mp_musicAudio;
  Phonon::MediaObject* mp_explodeAudio;
  Phonon::MediaObject* mp_thrustAudio;
  Phonon::MediaObject* mp_fireAudio;
  Phonon::MediaObject* mp_eventAudio;

  int m_txs() const;
  double m_scale() const;
  void m_recalcGameArea();
  void m_setFont(QPainter& p, asteroid::LFSize sz, bool bold = false);
  int m_textOut(QPainter& p, const QString& s, int x, int y,
    Qt::Alignment layout = Qt::AlignLeft | Qt::AlignTop);
  void m_resetScreen(QPainter& p);
  void m_drawPlay();
  void m_drawIntroScreen0();
  void m_drawIntroScreen1();
  void m_drawIntroScreen2();

  // Game key control id
  enum GameKey {GK_NONE = 0, GK_LEFT, GK_RIGHT, GK_THRUST, GK_FIRE};
  static GameKey sm_qkToGk(int qtk);

private slots:
  // Private slots
  void m_poll();
protected:
  // Protected members
  virtual void paintEvent(QPaintEvent* event);
public:

  // Construction
  GameWidget(QWidget* parent = 0);
  ~GameWidget();

  // Public members
  void pause(bool p);
  bool isPaused() const;
  void play(bool p);
  bool isPlaying() const;
  GameWidget::GameStateType gameState() const;
  void setGameState(GameWidget::GameStateType gs);
  int hiscore() const;
  void setHiscore(int hs);
  bool sounds() const;
  void setSounds(bool s);
  bool music() const;
  void setMusic(bool m);
  QColor background() const;
  void setBackground(const QColor& c);
  QColor foreground() const;
  void setForeground(const QColor& c);
  double difficulty() const;
  void setDifficulty(const double& dif);
  int pollRate() const;
  void setPollRate(int ms);
  bool keyDown(int k);
  bool keyUp(int k);
};

#endif // OnceOnly
