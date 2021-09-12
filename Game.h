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
   void _SetFont(QPainter &p, Asteroid::LFSize sz, bool bold = false);
   int _PutStr(QPainter &p, const QString &s, int x, int y, Qt::Alignment layout = Qt::AlignLeft | Qt::AlignTop);
   void _ResetScreen(QPainter &p);
   void _ShowPlay();
   void _ShowIntro0();
   void _ShowIntro1();
   void _ShowIntro2();
private slots:
   void _Poll();
protected:
   virtual void paintEvent(QPaintEvent *event);
public:
   Game(QWidget *parent = nullptr);
   ~Game();
   bool GetPausing() const;
   void SetPausing(bool p);
   bool GetPlaying() const;
   void SetPlaying(bool p);
   Game::StateT GetState() const;
   void SetState(Game::StateT gs);
   int GetHiScore() const;
   void SetHiScore(int hs);
   bool GetSounding() const;
   void SetSounding(bool s);
   bool GetSinging() const;
   void SetSinging(bool m);
   QColor GetColorFg() const;
   void SetColorFg(const QColor &c);
   QColor GetColorBg() const;
   void SetColorBg(const QColor &c);
   double GetLevel() const;
   void SetLevel(const double &dif);
   int GetPollRate() const;
   void SetPollRate(int ms);
   bool EnKey(int k);
   bool DeKey(int k);
};

#endif // OnceOnly
