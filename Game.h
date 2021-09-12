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
   typedef enum { Intro0Q = 0, Intro1Q = 1, Intro2Q = 2, PlayQ = 3, DemoQ = 4 } StateT;
private:
   bool _Pausing, _Sounding, _Singing, _Playing;
   bool _EnPause, _EnSound, _EnMusic;
   time_t _Time0;
   double _Arena;
   StateT _State;
   QColor _ColorFg, _ColorBg;
   QTimer *_Timer;
   Asteroid::Engine *_Machine;
// Sound players
   Phonon::MediaObject *_MusicWav, *_BoomWav, *_ThrustWav, *_FireWav, *_EventWav;
   int _Filler() const;
   double _Scaling() const;
   void _ResizeArena();
   void _SetFont(QPainter &Pnt, Asteroid::FontT N, bool Bold = false);
   int _PutStr(QPainter &Pnt, const QString &Str, int X, int Y, Qt::Alignment Grid = Qt::AlignLeft | Qt::AlignTop);
   void _ResetScreen(QPainter &Pnt);
   void _ShowPlay();
   void _ShowIntro0();
   void _ShowIntro1();
   void _ShowIntro2();
private slots:
   void _Poll();
protected:
   virtual void paintEvent(QPaintEvent *Ev);
public:
   Game(QWidget *Sup = nullptr);
   ~Game();
   bool GetPausing() const;
   void SetPausing(bool Pausing);
   bool GetPlaying() const;
   void SetPlaying(bool Playing);
   Game::StateT GetState() const;
   void SetState(Game::StateT Q);
   int GetHiScore() const;
   void SetHiScore(int Score);
   bool GetSounding() const;
   void SetSounding(bool Sounding);
   bool GetSinging() const;
   void SetSinging(bool Singing);
   QColor GetColorFg() const;
   void SetColorFg(const QColor &ColorFg);
   QColor GetColorBg() const;
   void SetColorBg(const QColor &ColorBg);
   double GetLevel() const;
   void SetLevel(const double &Level);
   int GetPollRate() const;
   void SetPollRate(int PollRate);
   bool EnKey(int Key);
   bool DeKey(int Key);
};

#endif // OnceOnly
