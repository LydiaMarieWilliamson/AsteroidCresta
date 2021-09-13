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

static const int DefPollRate = 45, IntroScreenTime = 8;
static const QString ScreenFontName = "serif";

// class Game: private members
// ───────────────────────────
// The scaled text spacer.
int Game::_Filler() const { return (int)(5.0*_Scaling()); }

// The scaling value based on the width.
double Game::_Scaling() const {
   int Xs; _Machine->GetPlayDims(&Xs, 0);
   return Xs > 0? (double)width()/Xs: 1.0;
}

// Resize the internal gaming area by adjusting its aspect ratio in such a way as to keep the area approximately constant.
// This is to be called when the parent's size is changed.
void Game::_ResizeArena() {
   int Xs = width(), Ys = height();
   if (Ys > 0) {
      double Aspect = (double)Xs/Ys;
      _Machine->SetPlayDims((int)sqrt(Aspect*_Arena), (int)((double)Xs/Aspect));
   }
}

// Set the painter font according to size N and boldness Bold.
void Game::_SetFont(QPainter &Pnt, Asteroid::FontT N, bool Bold/* = false*/) {
   double Pts;
   switch (N) {
      case Asteroid::SmallLF: Pts = 10; break;
      case Asteroid::LargeLF: Pts = 14; break;
      case Asteroid::HugeBoldLF: Pts = 16, Bold = true; break;
      default: Pts = 12; break;
   }
// Rescale the point size up to a fixed lower limit.
   if ((Pts *= _Scaling()) < 8) Pts = 8;
// Set the font.
   QFont Font = Pnt.font(); Font.setBold(Bold), Font.setPointSizeF(Pts), Pnt.setFont(Font);
}

// Draw the text out at X, Y; returning the height of the text drawn.
// LayOut options: AlignLeft, AlignRight, AlignHCenter, AlignTop, AlignBottom, AlignVCenter and AlignCenter; others are ignored.
// These define how the text is to be aligned to X and Y, rather than with respect to any rectangle.
// For example, if LayOut&AlignRight, the right hand edge of the text will be aligned to X.
int Game::_PutStr(QPainter &Pnt, const QString &Str, int X, int Y, Qt::Alignment LayOut) {
   QRect R = Pnt.boundingRect(rect(), 0, Str);
// Horizontal.
   if (LayOut&Qt::AlignRight) X -= R.width(); else if (LayOut&(Qt::AlignHCenter | Qt::AlignCenter)) X -= R.width()/2;
// Vertical.
   if (LayOut&Qt::AlignBottom) Y -= R.height(); else if (LayOut&(Qt::AlignVCenter | Qt::AlignCenter)) Y -= R.height()/2;
   Pnt.drawText(QRect(X, Y, R.width(), R.height()), Qt::AlignLeft | Qt::AlignTop, Str);
   return R.height();
}

// Render a blank painter and set up the colors.
void Game::_ResetScreen(QPainter &Pnt) {
   Pnt.setFont(QFont(ScreenFontName)), Pnt.setPen(QPen(_ColorFg)), Pnt.fillRect(rect(), _ColorBg);
}

// Draw the play action, including the demo phase, during active game play or demo mode to render the game engine objects.
void Game::_ShowPlay() {
   QPainter Pnt(this); _ResetScreen(Pnt);
   double Sc = _Scaling();
   for (size_t Ox = 0; Ox < _Machine->ObjN(); Ox++) {
   // For each live game object.
   // Get a reference to the game object.
      Asteroid::Thing *Obj = _Machine->ObjAtN(Ox); if (Obj->GetDead()) continue;
   // Draw the shape, if there is one.
      int N = Obj->GetPoints();
      if (N > 0) {
         int X0 = (int)(Sc*Obj->PosPoints(0).real()), Y0 = (int)(Sc*Obj->PosPoints(0).imag());
         for (int n = 1; n < N; n++) {
            int X = (int)(Sc*Obj->PosPoints(n).real()), Y = (int)(Sc*Obj->PosPoints(n).imag());
            Pnt.drawLine(X0, Y0, X, Y), X0 = X, Y0 = Y;
         }
      }
   // Add the labels.
      QString Str = tr(Obj->GetCaption().c_str());
   // Draw the new position, if there is one.
      if (!Str.isEmpty())
         _SetFont(Pnt, Obj->GetPts()),
         _PutStr(Pnt, Str, (int)(Sc*Obj->_Pos.real()), (int)(Sc*Obj->_Pos.imag()), Qt::AlignCenter);
   }
// Indicate paused, if applicable.
   if (_Pausing) _SetFont(Pnt, Asteroid::SmallLF), _PutStr(Pnt, tr("PAUSED"), width()/2, height()/2, Qt::AlignCenter);
// Mark the scores and lives.
   _SetFont(Pnt, Asteroid::SmallLF);
   int Sh = _PutStr(Pnt, tr("SCORE ") + QString::number(_Machine->GetScore()), _Filler(), _Filler());
   _PutStr(Pnt, tr("LIVES ") + QString::number(_Machine->GetLives()), _Filler(), _Filler() + Sh);
   Sh = _PutStr(Pnt, tr("HI SCORE ") + QString::number(_Machine->GetHiScore()), width() - _Filler(), _Filler(), Qt::AlignRight);
   _PutStr(Pnt, QString(_Machine->Charge(), '|'), _Filler(), height() - _Filler(), Qt::AlignBottom);
}

// Draw intro screen #0.
void Game::_ShowIntro0() {
   QPainter Pnt(this); _ResetScreen(Pnt);
   int Xs = width(), Ys = height(), Y = Ys/8;
// Titles.
   _SetFont(Pnt, Asteroid::HugeBoldLF);
   Y += _PutStr(Pnt, AppName.toUpper(), Xs/2, Y, Qt::AlignHCenter);
   _SetFont(Pnt, Asteroid::SmallLF);
   int dY = _PutStr(Pnt, AppCopyRight, Xs/2, Y, Qt::AlignHCenter);
   Y += 2*dY;
   _SetFont(Pnt, Asteroid::MediumLF);
   Y += _PutStr(Pnt, tr("INSERT COIN"), Xs/2, Y, Qt::AlignHCenter);
// The website string from the bottom of the page.
   int Yh = Y;
   Y = 7*Ys/8;
   _SetFont(Pnt, Asteroid::MediumLF);
   Y -= _PutStr(Pnt, AppDomain, Xs/2, Y, Qt::AlignHCenter | Qt::AlignBottom);
// Additional text (not shown if no room).
   int Yt = 4*dY;
   if (Yt < Y - Yh) {
      Y = Yh + (Y - Yh - Yt)/2;
      _SetFont(Pnt, Asteroid::SmallLF);
      Y += _PutStr(Pnt, tr("This game was inspired by Atari Asteroids--a classic from 1979."), Xs/2, Y, Qt::AlignHCenter);
      Y += _PutStr(Pnt, tr("It is written in C++ with a QT front-end. No warranty."), Xs/2, Y, Qt::AlignHCenter);
      Y += _PutStr(Pnt, tr("Released under GNU General Public License."), Xs/2, Y, Qt::AlignHCenter);
   }
}

// Draw intro screen #1.
void Game::_ShowIntro1() {
   QPainter Pnt(this); _ResetScreen(Pnt);
   int Xs = width(), Ys = height();
   int Y = Ys/8;
// Titles.
   _SetFont(Pnt, Asteroid::HugeBoldLF);
   Y += 2*_PutStr(Pnt, AppName.toUpper(), Xs/2, Y, Qt::AlignHCenter);
   _SetFont(Pnt, Asteroid::MediumLF);
   Y += _PutStr(Pnt, tr("CONTROLS"), Xs/2, Y, Qt::AlignHCenter);
// Keys.
   _SetFont(Pnt, Asteroid::SmallLF);
   Y += _PutStr(Pnt, tr("L ARROW (or K) - Rotate Left"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("R ARROW (or L) - Rotate Right"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("UP ARROW (or A) - Thrust"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("CTRL (or SPACE) - Fire"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("P - Pause"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("ESC - Quit Game"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr(" "), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("S - Toggle Game Sounds ") + tr(_Sounding? "(ON)": "(OFF)"), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("M - Toggle Music ") + tr(_Singing? "(ON)": "(OFF)"), Xs/2, Y, Qt::AlignHCenter);
// The copyright string from the bottom of the page.
   int Yh = Y;
   Y = Ys - _Filler();
#if 0
//(@) Redundant, over-booked in size, and therefore removed.
   _SetFont(Pnt, Asteroid::SmallLF);
   _PutStr(Pnt, AppCopyRight, _Filler(), Y, Qt::AlignBottom);
   Y -= _PutStr(Pnt, AppDomain, Xs - _Filler(), Y, Qt::AlignRight | Qt::AlignBottom);
#endif
// Additional text (not shown if there is no room).
   _SetFont(Pnt, Asteroid::MediumLF);
   int dY = _PutStr(Pnt, tr(" "), Xs/2, Y, Qt::AlignHCenter | Qt::AlignBottom);
   if (dY < Y - Yh) _PutStr(Pnt, tr("Press SPACE to Play"), Xs/2, Yh + (Y - Yh - dY)/2, Qt::AlignHCenter);
}

// Draw intro screen #2.
void Game::_ShowIntro2() {
   QPainter Pnt(this); _ResetScreen(Pnt);
   int Xs = width(), Ys = height();
   int Y = Ys/8;
// Hi Score.
   _SetFont(Pnt, Asteroid::HugeBoldLF);
   Y += 3*_PutStr(Pnt, AppName.toUpper(), Xs/2, Y, Qt::AlignHCenter);
   _SetFont(Pnt, Asteroid::MediumLF);
   Y += _PutStr(Pnt, tr("HIGHEST SCORE : ") + QString::number(_Machine->GetHiScore()), Xs/2, Y, Qt::AlignHCenter);
   Y += _PutStr(Pnt, tr("LAST SCORE : ") + QString::number(_Machine->GetExScore()), Xs/2, Y, Qt::AlignHCenter);
// The copyright string from the bottom of the page.
   int Yh = Y;
   Y = Ys - _Filler();
#if 0
//(@) Redundant, over-booked in size, and therefore removed.
   _SetFont(Pnt, Asteroid::SmallLF);
   _PutStr(Pnt, AppCopyRight, _Filler(), Y, Qt::AlignBottom);
   Y -= _PutStr(Pnt, AppDomain, Xs - _Filler(), Y, Qt::AlignRight | Qt::AlignBottom);
#endif
// Additional text (not shown if there's no room).
   _SetFont(Pnt, Asteroid::MediumLF);
   int dY = _PutStr(Pnt, tr(" "), Xs/2, Y, Qt::AlignHCenter | Qt::AlignBottom);
   if (dY < Y - Yh) _PutStr(Pnt, tr("Press SPACE to Play"), Xs/2, Yh + (Y - Yh - dY)/2, Qt::AlignHCenter);
}

// class Game: private slots
// ─────────────────────────
// Internal poller.
// Called repeatedly to update the game state; also responsible for paging through the intro screens.
void Game::_Poll() {
// The media file's pathname.
   const QString Path = QCoreApplication::applicationDirPath() + "/Media/";
   if (_Machine->GetActive()) {
   // The game is active, i.e. in play or showing a demo.
   // Update the game state for the next poll, if active.
      if (!_Pausing) _Machine->Tick();
   // Move directly to the intro screen at the end of the game,
   // otherwise repaint the updated state coming from the last Tick() call.
      if (_Machine->EndGame()) SetState(Intro0Q); else update();
      if (_Sounding && _Machine->InGame()) {
         switch (_Machine->GetBoomSnd()) {
            case Asteroid::BoulderOT:
               _BoomWav->setCurrentSource(PhononFile(Path, "Boom.wav")), _BoomWav->play();
            break;
            case Asteroid::StoneOT:
               _BoomWav->setCurrentSource(PhononFile(Path, "Blast.wav")), _BoomWav->play();
            break;
            case Asteroid::PebbleOT:
               _BoomWav->setCurrentSource(PhononFile(Path, "Pop.wav")), _BoomWav->play();
            break;
            default: break;
         }
         if (_Machine->GetLanceSnd()) _FireWav->setCurrentSource(PhononFile(Path, "Fire.wav")), _FireWav->play();
         if (!_Machine->GetThrustSnd()) _ThrustWav->stop();
         else if (_ThrustWav->state() != Phonon::PlayingState && _ThrustWav->state() != Phonon::BufferingState)
            _ThrustWav->setCurrentSource(PhononFile(Path, "Thrust.wav")), _ThrustWav->play();
         if (_Machine->GetAlienSnd()) _EventWav->setCurrentSource(PhononFile(Path, "Alien.wav")), _EventWav->play();
         if (_Machine->GetDiedSnd()) _EventWav->setCurrentSource(PhononFile(Path, "Die.wav")), _EventWav->play();
      }
   } else if (time(0) >= _Time0 + IntroScreenTime)
   // Rotate the intro screens, including any change from the intro to the demo state.
      switch (_State) {
         case Intro0Q: SetState(Intro1Q); break;
         case Intro1Q: SetState(Intro2Q); break;
         case Intro2Q: SetState(DemoQ); break;
         default: SetState(Intro0Q); break;
      }
// Start the music, change the track or stop the music.
   if (_Singing && (_Playing != _Machine->InGame() || (_MusicWav->state() != Phonon::BufferingState && _MusicWav->state() != Phonon::PlayingState)))
      _MusicWav->setCurrentSource(PhononFile(Path, _Machine->InGame()? "Play.mp3": "Intro.mp3")), _MusicWav->play();
   else if (!_Singing && _MusicWav->state() == Phonon::PlayingState)
      _MusicWav->stop();
// Stop any lingering sounds.
   if ((!_Sounding || !_Machine->InGame()) && _ThrustWav->state() == Phonon::PlayingState) _ThrustWav->stop();
// Hold the last state to detect any change.
   _Playing = _Machine->InGame();
}

// class Game: protected members
// ─────────────────────────────
// The paint event handler: call the appropriate rendering method.
void Game::paintEvent(QPaintEvent * /*Ev*/) {
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
Game::Game(QWidget *Sup): QWidget(Sup, Qt::Widget) {
   _Pausing = false, _Sounding = true, _Singing = true, _Playing = false;
   _EnSound = false, _EnMusic = false, _EnPause = false;
   _ColorFg = Qt::white, _ColorBg = Qt::black;
   _State = Intro0Q, _Time0 = time(0);
// Create the media players.
   _MusicWav = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource());
   _BoomWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   _ThrustWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   _FireWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   _EventWav = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
// Set up the game engine.
   _Machine = new Asteroid::Engine();
   int Xs, Ys; _Machine->GetPlayDims(&Xs, &Ys);
   _Arena = Xs*Ys, _ResizeArena();
// Set up the poll timer.
   _Timer = new QTimer(this), connect(_Timer, SIGNAL(timeout()), this, SLOT(_Poll())), _Timer->start(DefPollRate);
}

// Free the Game object.
Game::~Game() {
   try {
      delete _Machine; delete _MusicWav; delete _BoomWav;
      delete _ThrustWav; delete _FireWav; delete _EventWav;
   } catch(...) { }
}

// Get/set the game-pause state.
bool Game::GetPausing() const { return _Pausing; }
void Game::SetPausing(bool Pausing) { _Pausing = Pausing && GetPlaying(); }

// Get/set the game-playing state.
bool Game::GetPlaying() const { return GetState() == PlayQ; }
void Game::SetPlaying(bool Playing) {
   if (Playing != GetPlaying()) SetState(Playing? PlayQ: Intro0Q);
}

// Get/set the game state.
// Setting can be used to start a game, a demo or to change the intro screen.
// All changes to the game state should go through these procedures.
Game::StateT Game::GetState() const { return _State; }

void Game::SetState(Game::StateT State) {
   if (State != _State) {
   // Start engine playing, engine demo or kill play/demo.
      switch (State) {
         case PlayQ: _Pausing = false, _Machine->BegGame(); break;
         case DemoQ: _Machine->BegDemo(); break;
         default: _Pausing = false, _Machine->Stop(); break;
      }
   // Update the state and hold the time when it was done.
      _State = State, _Time0 = time(0), update();
   }
}

// Get/set the high score.
int Game::GetHiScore() const { return _Machine->GetHiScore(); }
void Game::SetHiScore(int Score) { _Machine->SetHiScore(Score); }

// Get/set the sounding/singing states; foreground/background colors.
// Update the intro pages after any change is made.
bool Game::GetSounding() const { return _Sounding; }
void Game::SetSounding(bool Sounding) {
   if (_Sounding != Sounding) _Sounding = Sounding, update();
}
bool Game::GetSinging() const { return _Singing; }
void Game::SetSinging(bool Singing) {
   if (_Singing != Singing) _Singing = Singing, update();
}
QColor Game::GetColorFg() const { return _ColorFg; }
void Game::SetColorFg(const QColor &ColorFg) {
   if (_ColorFg != ColorFg) _ColorFg = ColorFg, update();
}
QColor Game::GetColorBg() const { return _ColorBg; }
void Game::SetColorBg(const QColor &ColorBg) {
   if (_ColorBg != ColorBg) _ColorBg = ColorBg, update();
}

// Get/set the game level.
// The level ∈ [0,1] determines how fast rocks are created; 0 = easiest, 1 = hardest.
double Game::GetLevel() const { return _Machine->GetLevel(); }
void Game::SetLevel(const double &Level) { _Machine->SetLevel(Level); }

// Get/set the game speed; i.e. the polling rate, which is in milliseconds.
int Game::GetPollRate() const { return _Timer->interval(); }
void Game::SetPollRate(int PollRate) { _Timer->setInterval(PollRate); }

// Handle a key down event; meant to be called from outside this class in response to key events.
// Return true if handled.
bool Game::EnKey(int Key) {
   switch (Key) {
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
         if (!_EnSound) SetSounding(!_Sounding), _EnSound = true;
      return true;
   // Music key down: toggle the music state.
      case Qt::Key_M:
         if (!_EnMusic) SetSinging(!_Singing), _EnMusic = true;
      return true;
   // Pause key down: toggle the pause state.
      case Qt::Key_P:
         if (!_EnPause) SetPausing(!_Pausing), _EnPause = true;
      return true;
   }
   return false;
}

// Handle a key up event; meant to be called from outside this class in response to key events.
// Return true if handled.
bool Game::DeKey(int Key) {
   switch (Key) {
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
      case Qt::Key_S: _EnSound = false; return true;
   // Music key up.
      case Qt::Key_M: _EnMusic = false; return true;
   // Pause key up.
      case Qt::Key_P: _EnPause = false; return true;
   }
   return false;
}
