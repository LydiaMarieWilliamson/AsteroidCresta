// Asteroid Style Game: The gaming visible area.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QtGui>
#include <phonon>
#include <math.h>
#include "Game.h"
#include "Engine.h"
#include "Version.h"

// QT and phonon changed.
#if 1
#   define PhononFile(Path, File)	(Phonon::MediaSource(QUrl::fromLocalFile((Path) + (File))))
#else
#   define PhononFile(Path, File)	(Phonon::MediaSource((Path) + (File)))
#endif

static const int DEF_POLL_RATE = 45, INTRO_SCREEN_SEC = 8;
static const QString SCREEN_FONT_NAME = "serif";

// class Game: private members
// ───────────────────────────
// The scaled text spacer.
int Game::_Filler() const {
   return (int)(5.0*_Scaling());
}

// The scaling value based on the width.
double Game::_Scaling() const {
   int gw;
   _Machine->GetPlayDims(&gw, 0);
   return gw > 0? (double)width()/gw: 1.0;
}

// Resize the internal gaming area by adjusting its aspect ratio in such a way as to keep the area approximately constant.
// This is to be called when the parent's size is changed.
void Game::_ResizeArena() {
   int w = width();
   int h = height();

   if (h > 0) {
      double asr = (double)w/h;
      w = (int)sqrt(asr*_Arena);
      h = (int)((double)w/asr);
      _Machine->SetPlayDims(w, h);
   }
}

// Set the painter font according to size sz and boldness bold.
void Game::_SetFont(QPainter &p, Asteroid::LFSize sz, bool bold) {
   double ps;
   QFont f = p.font();

   switch (sz) {
      case Asteroid::lfSmall: ps = 10; break;
      case Asteroid::lfLarge: ps = 14; break;
      case Asteroid::lfHugeBold: ps = 16, bold = true; break;
      default: ps = 12; break;
   }

// Rescale the point size up to a fixed lower limit.
   ps *= _Scaling();
   if (ps < 8) ps = 8;

// Set the font.
   f.setBold(bold);
   f.setPointSizeF(ps);
   p.setFont(f);
}

// Draw the text out at x, y; returning the height of the text drawn.
// Layout options: AlignLeft, AlignRight, AlignHCenter, AlignTop, AlignBottom, AlignVCenter and AlignCenter; others are ignored.
// These define how the text is to be aligned to x and y, rather than with respect to any rectangle.
// For example, if layout&AlignRight, the right hand edge of the text will be aligned to x.
int Game::_PutStr(QPainter &p, const QString &s, int x, int y, Qt::Alignment layout) {
   QRect tr = p.boundingRect(rect(), 0, s);

// Horizontal.
   if (layout&Qt::AlignRight) {
      x -= tr.width();
   } else if (layout&(Qt::AlignHCenter | Qt::AlignCenter)) {
      x -= tr.width()/2;
   }
// Vertical.
   if (layout&Qt::AlignBottom) {
      y -= tr.height();
   } else if (layout&(Qt::AlignVCenter | Qt::AlignCenter)) {
      y -= tr.height()/2;
   }

   p.drawText(QRect(x, y, tr.width(), tr.height()), Qt::AlignLeft | Qt::AlignTop, s);
   return tr.height();
}

// Render a blank painter and set up the colors.
void Game::_ResetScreen(QPainter &p) {
   p.setFont(QFont(SCREEN_FONT_NAME));
   p.setPen(QPen(_ColorFg));
   p.fillRect(rect(), _ColorBg);
}

// Draw the play action, including the demo phase, during active game play or demo mode to render the game engine objects.
void Game::_ShowPlay() {
   QPainter p(this);
   _ResetScreen(p);

   int xo, yo, x, y;
   double sc = _Scaling();

   for (size_t oidx = 0; oidx < _Machine->ObjN(); ++oidx) {
   // For each live game object.
   // Get a reference to the game object.
      Asteroid::Obj *obj = _Machine->ObjAtN(oidx);
      if (obj->dead()) continue;
   // Draw the shape, if there is one.
      int pc = obj->pointCnt();

      if (pc > 0) {
         xo = (int)(sc*obj->points(0).real());
         yo = (int)(sc*obj->points(0).imag());

         for (int n = 1; n < pc; ++n) {
            x = (int)(sc*obj->points(n).real());
            y = (int)(sc*obj->points(n).imag());

            p.drawLine(xo, yo, x, y);

            xo = x;
            yo = y;
         }
      }
   // Add the labels.
      QString s = tr(obj->caption().c_str());

   // Draw the new position, if there is one.
      if (!s.isEmpty()) {
         x = (int)(sc*obj->pos.real());
         y = (int)(sc*obj->pos.imag());

         _SetFont(p, obj->fontSize());
         _PutStr(p, s, x, y, Qt::AlignCenter);
      }
   }

// Indicate paused, if applicable.
   if (_Pausing) {
      _SetFont(p, Asteroid::lfSmall);
      _PutStr(p, tr("PAUSED"), width()/2, height()/2, Qt::AlignCenter);
   }
// Mark the scores and lives.
   _SetFont(p, Asteroid::lfSmall);
   int sh = _PutStr(p, tr("SCORE ") + QString::number(_Machine->GetScore()),
      _Filler(), _Filler());

   _PutStr(p, tr("LIVES ") + QString::number(_Machine->GetLives()), _Filler(), _Filler() + sh);

   sh = _PutStr(p, tr("HI SCORE ") + QString::number(_Machine->GetHiScore()), width() - _Filler(), _Filler(), Qt::AlignRight);

   _PutStr(p, QString(_Machine->Charge(), '|'), _Filler(), height() - _Filler(), Qt::AlignBottom);
}

// Draw intro screen #0.
void Game::_ShowIntro0() {
   QPainter p(this);
   _ResetScreen(p);

   int w = width();
   int h = height();

   int y = h/8;

// Titles.
   _SetFont(p, Asteroid::lfHugeBold);
   y += _PutStr(p, AppName.toUpper(), w/2, y, Qt::AlignHCenter);

   _SetFont(p, Asteroid::lfSmall);
   int th = _PutStr(p, AppCopyRight, w/2, y, Qt::AlignHCenter);

   y += 2*th;
   _SetFont(p, Asteroid::lfMedium);
   y += _PutStr(p, tr("INSERT COIN"), w/2, y, Qt::AlignHCenter);

// The website string from the bottom of the page.
   int hy = y;
   y = 7*h/8;
   _SetFont(p, Asteroid::lfMedium);
   y -= _PutStr(p, AppDomain, w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

// Additional text (not shown if no room).
   int tlh = 4*th;

   if (tlh < y - hy) {
      y = hy + (y - hy - tlh)/2;

      _SetFont(p, Asteroid::lfSmall);
      y += _PutStr(p, tr("This game was inspired by Atari Asteroids--a classic from 1979."), w/2, y, Qt::AlignHCenter);
      y += _PutStr(p, tr("It is written in C++ with a QT front-end. No warranty."), w/2, y, Qt::AlignHCenter);
      y += _PutStr(p, tr("Released under GNU General Public License."), w/2, y, Qt::AlignHCenter);
   }

}

// Draw intro screen #1.
void Game::_ShowIntro1() {
   QPainter p(this);
   _ResetScreen(p);

   int w = width();
   int h = height();

   int y = h/8;

// Titles.
   _SetFont(p, Asteroid::lfHugeBold);
   int th = _PutStr(p, AppName.toUpper(), w/2, y, Qt::AlignHCenter);

   y += 2*th;
   _SetFont(p, Asteroid::lfMedium);
   y += _PutStr(p, tr("CONTROLS"), w/2, y, Qt::AlignHCenter);

// Keys.
   _SetFont(p, Asteroid::lfSmall);
   y += _PutStr(p, tr("L ARROW (or K) - Rotate Left"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("R ARROW (or L) - Rotate Right"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("UP ARROW (or A) - Thrust"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("CTRL (or SPACE) - Fire"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("P - Pause"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("ESC - Quit Game"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr(" "), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("S - Toggle Game Sounds ") + tr(_Sounding? "(ON)": "(OFF)"), w/2, y, Qt::AlignHCenter);
   y += _PutStr(p, tr("M - Toggle Music ") + tr(_Singing? "(ON)": "(OFF)"), w/2, y, Qt::AlignHCenter);

// The copyright string from the bottom of the page.
   int hy = y;
   y = h - _Filler();
#if 0
//(@) Redundant, over-booked in size, and therefore removed.
   _SetFont(p, Asteroid::lfSmall);
   _PutStr(p, AppCopyRight, _Filler(), y, Qt::AlignBottom);
   y -= _PutStr(p, AppDomain, w - _Filler(), y, Qt::AlignRight | Qt::AlignBottom);
#endif

// Additional text (not shown if there is no room).
   _SetFont(p, Asteroid::lfMedium);
   th = _PutStr(p, tr(" "), w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

   if (th < y - hy) {
      y = hy + (y - hy - th)/2;
      _PutStr(p, tr("Press SPACE to Play"), w/2, y, Qt::AlignHCenter);
   }
}

// Draw intro screen #2.
void Game::_ShowIntro2() {
   QPainter p(this);
   _ResetScreen(p);

   int w = width();
   int h = height();

   int y = h/8;

// Hi Score.
   _SetFont(p, Asteroid::lfHugeBold);
   int th = _PutStr(p, AppName.toUpper(), w/2, y, Qt::AlignHCenter);

   y += 3*th;
   _SetFont(p, Asteroid::lfMedium);
   y += _PutStr(p, tr("HIGHEST SCORE : ") + QString::number(_Machine->GetHiScore()), w/2, y, Qt::AlignHCenter);

   y += _PutStr(p, tr("LAST SCORE : ") + QString::number(_Machine->GetExScore()), w/2, y, Qt::AlignHCenter);

// The copyright string from the bottom of the page.
   int hy = y;
   y = h - _Filler();
#if 0
//(@) Redundant, over-booked in size, and therefore removed.
   _SetFont(p, Asteroid::lfSmall);
   _PutStr(p, AppCopyRight, _Filler(), y, Qt::AlignBottom);
   y -= _PutStr(p, AppDomain, w - _Filler(), y, Qt::AlignRight | Qt::AlignBottom);
#endif

// Additional text (not shown if there's no room).
   _SetFont(p, Asteroid::lfMedium);
   th = _PutStr(p, tr(" "), w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

   if (th < y - hy) {
      y = hy + (y - hy - th)/2;
      _PutStr(p, tr("Press SPACE to Play"), w/2, y, Qt::AlignHCenter);
   }
}

// class Game: private slots
// ─────────────────────────
// Internal poller.
// Called repeatedly to update the game state; also responsible for paging through the intro screens.
void Game::_Poll() {
// The media file's pathname.
   const QString path = QCoreApplication::applicationDirPath() + "/media/";

   if (_Machine->GetActive()) {
   // The game is active, i.e. in play or showing a demo.
   // Update the game state for the next poll, if active.
      if (!_Pausing) {
         _Machine->Tick();
      }

   // Move directly to the intro screen at the end of the game,
   // otherwise repaint the updated state coming from the last Tick() call.
      if (_Machine->EndGame()) {
         SetState(Intro0Q);
      } else {
         update();
      }

      if (_Sounding && _Machine->InGame()) {
         switch (_Machine->GetBoomSnd()) {
            case Asteroid::otBigRock:
               _BoomWav->setCurrentSource(PhononFile(path, "explode_large.wav")), _BoomWav->play();
            break;
            case Asteroid::otMedRock:
               _BoomWav->setCurrentSource(PhononFile(path, "explode_medium.wav")), _BoomWav->play();
            break;
            case Asteroid::otSmallRock:
               _BoomWav->setCurrentSource(PhononFile(path, "explode_small.wav")), _BoomWav->play();
            break;
            default: break;
         }

         if (_Machine->GetLanceSnd())
            _FireWav->setCurrentSource(PhononFile(path, "fire.wav")), _FireWav->play();

         if (!_Machine->GetThrustSnd())
            _ThrustWav->stop();
         else if (_ThrustWav->state() != Phonon::PlayingState && _ThrustWav->state() != Phonon::BufferingState)
            _ThrustWav->setCurrentSource(PhononFile(path, "thrust.wav")), _ThrustWav->play();

         if (_Machine->GetAlienSnd())
            _EventWav->setCurrentSource(PhononFile(path, "alien.wav")), _EventWav->play();

         if (_Machine->GetDiedSnd())
            _EventWav->setCurrentSource(PhononFile(path, "die.wav")), _EventWav->play();
      }
   } else if (time(0) >= _Time0 + INTRO_SCREEN_SEC) {
   // Rotate the intro screens, including any change from the intro to the demo state.
      switch (_State) {
         case Intro0Q: SetState(Intro1Q); break;
         case Intro1Q: SetState(Intro2Q); break;
         case Intro2Q: SetState(DemoQ); break;
         default: SetState(Intro0Q); break;
      }
   }

// Start the music, change the track or stop the music.
   if (_Singing && (_Playing != _Machine->InGame() || (_MusicWav->state() != Phonon::BufferingState && _MusicWav->state() != Phonon::PlayingState))) {
      _MusicWav->setCurrentSource(PhononFile(path, _Machine->InGame()? "play.mp3": "intro.mp3"));
      _MusicWav->play();
   } else if (!_Singing && _MusicWav->state() == Phonon::PlayingState) {
      _MusicWav->stop();
   }
// Stop any lingering sounds.
   if ((!_Sounding || !_Machine->InGame()) && _ThrustWav->state() == Phonon::PlayingState) {
      _ThrustWav->stop();
   }
// Hold the last state to detect any change.
   _Playing = _Machine->InGame();
}

// class Game: protected members
// ─────────────────────────────
// The paint event handler: call the appropriate rendering method.
void Game::paintEvent(QPaintEvent *) {
   _ResizeArena();

   switch (_State) {
      case Intro0Q: _ShowIntro0(); break;
      case Intro1Q: _ShowIntro1(); break;
      case Intro2Q: _ShowIntro2(); break;
      default: _ShowPlay(); break;
   }
}

// class Game: public members
// ──────────────────────────
// Make a new Game object.
Game::Game(QWidget *parent): QWidget(parent, Qt::Widget) {
   _Pausing = false, _Sounding = true, _Singing = true, _Playing = false;
   _EnPause = false, _EnSound = false, _EnMusic = false;
   _ColorBg = Qt::black, _ColorFg = Qt::white;
   _State = Intro0Q, _Time0 = time(0);

// Create the media players.
   _MusicWav = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource());
   _BoomWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   _ThrustWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   _FireWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   _EventWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());

// Set up the game engine.
   int w, h;
   _Machine = new Asteroid::Engine();
   _Machine->GetPlayDims(&w, &h);
   _Arena = w*h;
   _ResizeArena();

// Set up the poll timer.
   _Timer = new QTimer(this);
   connect(_Timer, SIGNAL(timeout()), this, SLOT(_Poll()));
   _Timer->start(DEF_POLL_RATE);
}

// Free the Game object.
Game::~Game() {
   try {
      delete _Machine;
      delete _MusicWav;
      delete _BoomWav;
      delete _ThrustWav;
      delete _FireWav;
      delete _EventWav;
   } catch(...) { }
}

// Get/set the game-pause state.
bool Game::GetPausing() const {
   return _Pausing;
}

void Game::SetPausing(bool p) {
   _Pausing = p && GetPlaying();
}

// Get/set the game-playing state.
void Game::SetPlaying(bool p) {
   if (p != GetPlaying()) SetState(p? PlayQ: Intro0Q);
}

bool Game::GetPlaying() const {
   return GetState() == PlayQ;
}

// Get/set the game state.
// Setting can be used to start a game, a demo or to change the intro screen.
// All changes to the game state should go through these procedures.
Game::StateT Game::GetState() const {
   return _State;
}

void Game::SetState(Game::StateT gs) {
   if (gs != _State) {
   // Start engine playing, engine demo or kill play/demo.
      if (gs == PlayQ) {
         _Pausing = false;
         _Machine->BegGame();
      } else if (gs == DemoQ) {
         _Machine->BegDemo();
      } else {
         _Pausing = false;
         _Machine->Stop();
      }

   // Update the state and hold the time when it was done.
      _State = gs;
      _Time0 = time(0);
      update();
   }
}

// Get/set the high score.
int Game::GetHiScore() const {
   return _Machine->GetHiScore();
}

void Game::SetHiScore(int hs) {
   _Machine->SetHiScore(hs);
}

// Get/set the sounding/singing states; foreground/background colors.
// Update the intro pages after any change is made.
bool Game::GetSounding() const {
   return _Sounding;
}

void Game::SetSounding(bool s) {
   if (_Sounding != s) {
      _Sounding = s;

      update();
   }
}

bool Game::GetSinging() const {
   return _Singing;
}

void Game::SetSinging(bool m) {
   if (_Singing != m) {
      _Singing = m;

      update();
   }
}

QColor Game::GetColorFg() const {
   return _ColorFg;
}

void Game::SetColorFg(const QColor &c) {
   if (_ColorFg != c) {
      _ColorFg = c;
      update();
   }
}

QColor Game::GetColorBg() const {
   return _ColorBg;
}

void Game::SetColorBg(const QColor &c) {
   if (_ColorBg != c) {
      _ColorBg = c;
      update();
   }
}

// Get/set the game level.
// The level ∈ [0,1] determines how fast rocks are created; 0 = easiest, 1 = hardest.
double Game::GetLevel() const {
   return _Machine->GetLevel();
}

void Game::SetLevel(const double &dif) {
   _Machine->SetLevel(dif);
}

// Get/set the game speed; i.e. the polling rate, which is in milliseconds.
int Game::GetPollRate() const {
   return _Timer->interval();
}

void Game::SetPollRate(int ms) {
   _Timer->setInterval(ms);
}

// Handle a key down event; meant to be called from outside this class in response to key events.
// Return true if handled.
bool Game::EnKey(int k) {
   switch (k) {
   // Game control keys down.
      case Qt::Key_K: case Qt::Key_Left: _Machine->SetSpin(-1); return true;
      case Qt::Key_L: case Qt::Key_Right: _Machine->SetSpin(+1); return true;
      case Qt::Key_A: case Qt::Key_Up: _Machine->SetPushing(true); return true;
      case Qt::Key_Control: case Qt::Key_Space: _Machine->Fire(); return true;
#if 0
   // Start game key down: start the game.
      case Qt::Key_Space: SetPlaying(true); return true;
#endif
   // Stop game key down: stop the game.
      case Qt::Key_Escape: SetPlaying(false); return true;
   // Sound key down: toggle the sound state.
      case Qt::Key_S:
         if (!_EnPause) {
            SetSounding(!_Sounding);
            _EnPause = true;
         }
      return true;
   // Music key down: toggle the music state.
      case Qt::Key_M:
         if (!_EnSound) {
            SetSinging(!_Sounding);
            _EnSound = true;
         }
      return true;
   // Pause key down: toggle the pause state.
      case Qt::Key_P:
         if (!_EnMusic) {
            SetPausing(!_Pausing);
            _EnMusic = true;
         }
      return true;
   }
   return false;
}

// Handle a key up event; meant to be called from outside this class in response to key events.
// Return true if handled.
bool Game::DeKey(int k) {
   switch (k) {
   // Game control keys up.
      case Qt::Key_K: case Qt::Key_Left: _Machine->SetSpin(0); return true;
      case Qt::Key_L: case Qt::Key_Right: _Machine->SetSpin(0); return true;
      case Qt::Key_A: case Qt::Key_Up: _Machine->SetPushing(false); return true;
      case Qt::Key_Control: case Qt::Key_Space: _Machine->ReLoad(); return true;
#if 0
   // Start game key up.
      case Qt::Key_Space: return true;
#endif
   // Stop game key up.
      case Qt::Key_Escape: return true;
   // Sound key up.
      case Qt::Key_S: _EnPause = false; return true;
   // Music key up.
      case Qt::Key_M: _EnSound = false; return true;
   // Pause key up.
      case Qt::Key_P: _EnMusic = false; return true;
   }
   return false;
}
