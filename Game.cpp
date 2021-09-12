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

const int DEF_POLL_RATE = 45;
const int INTRO_SCREEN_SEC = 8;
const QString SCREEN_FONT_NAME = "serif";

// class Game: private members
// ───────────────────────────
// The scaled text spacer.
int Game::m_txs() const {
   return (int)(5.0*m_scale());
}

// The scaling value based on the width.
double Game::m_scale() const {
   int gw;
   mp_engine->getPlayDims(&gw, 0);
   return gw > 0? (double)width()/gw: 1.0;
}

// Resize the internal gaming area by adjusting its aspect ratio in such a way as to keep the area approximately constant.
// This is to be called when the parent's size is changed.
void Game::m_recalcGameArea() {
   int w = width();
   int h = height();

   if (h > 0) {
      double asr = (double)w/h;
      w = (int)sqrt(asr*m_defaultGameArea);
      h = (int)((double)w/asr);
      mp_engine->setPlayDims(w, h);
   }
}

// Set the painter font according to size sz and boldness bold.
void Game::m_setFont(QPainter &p, asteroid::LFSize sz, bool bold) {
   double ps;
   QFont f = p.font();

   switch (sz) {
      case asteroid::lfSmall: ps = 10; break;
      case asteroid::lfLarge: ps = 14; break;
      case asteroid::lfHugeBold: ps = 16, bold = true; break;
      default: ps = 12; break;
   }

// Rescale the point size up to a fixed lower limit.
   ps *= m_scale();
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
int Game::m_textOut(QPainter &p, const QString &s, int x, int y, Qt::Alignment layout) {
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
void Game::m_resetScreen(QPainter &p) {
   p.setFont(QFont(SCREEN_FONT_NAME));
   p.setPen(QPen(m_foreCol));
   p.fillRect(rect(), m_backCol);
}

// Draw the play action, including the demo phase, during active game play or demo mode to render the game engine objects.
void Game::m_drawPlay() {
   QPainter p(this);
   m_resetScreen(p);

   int xo, yo, x, y;
   double sc = m_scale();

   for (size_t oidx = 0; oidx < mp_engine->objCnt(); ++oidx) {
   // For each live game object.
   // Get a reference to the game object.
      asteroid::Obj *obj = mp_engine->objAtIdx(oidx);
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

         m_setFont(p, obj->fontSize());
         m_textOut(p, s, x, y, Qt::AlignCenter);
      }
   }

// Indicate paused, if applicable.
   if (m_paused) {
      m_setFont(p, asteroid::lfSmall);
      m_textOut(p, tr("PAUSED"), width()/2, height()/2, Qt::AlignCenter);
   }
// Mark the scores and lives.
   m_setFont(p, asteroid::lfSmall);
   int sh = m_textOut(p, tr("SCORE ") + QString::number(mp_engine->score()),
      m_txs(), m_txs());

   m_textOut(p, tr("LIVES ") + QString::number(mp_engine->lives()), m_txs(), m_txs() + sh);

   sh = m_textOut(p, tr("HI SCORE ") + QString::number(mp_engine->hiscore()), width() - m_txs(), m_txs(), Qt::AlignRight);

   m_textOut(p, QString(mp_engine->charge(), '|'), m_txs(), height() - m_txs(), Qt::AlignBottom);
}

// Draw intro screen #0.
void Game::m_drawIntroScreen0() {
   QPainter p(this);
   m_resetScreen(p);

   int w = width();
   int h = height();

   int y = h/8;

// Titles.
   m_setFont(p, asteroid::lfHugeBold);
   y += m_textOut(p, AppName.toUpper(), w/2, y, Qt::AlignHCenter);

   m_setFont(p, asteroid::lfSmall);
   int th = m_textOut(p, AppCopyRight, w/2, y, Qt::AlignHCenter);

   y += 2*th;
   m_setFont(p, asteroid::lfMedium);
   y += m_textOut(p, tr("INSERT COIN"), w/2, y, Qt::AlignHCenter);

// The website string from the bottom of the page.
   int hy = y;
   y = 7*h/8;
   m_setFont(p, asteroid::lfMedium);
   y -= m_textOut(p, AppDomain, w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

// Additional text (not shown if no room).
   int tlh = 4*th;

   if (tlh < y - hy) {
      y = hy + (y - hy - tlh)/2;

      m_setFont(p, asteroid::lfSmall);
      y += m_textOut(p, tr("This game was inspired by Atari Asteroids--a classic from 1979."), w/2, y, Qt::AlignHCenter);
      y += m_textOut(p, tr("It is written in C++ with a QT front-end. No warranty."), w/2, y, Qt::AlignHCenter);
      y += m_textOut(p, tr("Released under GNU General Public License."), w/2, y, Qt::AlignHCenter);
   }

}

// Draw intro screen #1.
void Game::m_drawIntroScreen1() {
   QPainter p(this);
   m_resetScreen(p);

   int w = width();
   int h = height();

   int y = h/8;

// Titles.
   m_setFont(p, asteroid::lfHugeBold);
   int th = m_textOut(p, AppName.toUpper(), w/2, y, Qt::AlignHCenter);

   y += 2*th;
   m_setFont(p, asteroid::lfMedium);
   y += m_textOut(p, tr("CONTROLS"), w/2, y, Qt::AlignHCenter);

// Keys.
   m_setFont(p, asteroid::lfSmall);
   y += m_textOut(p, tr("L ARROW (or K) - Rotate Left"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("R ARROW (or L) - Rotate Right"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("UP ARROW (or A) - Thrust"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("CTRL (or SPACE) - Fire"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("P - Pause"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("ESC - Quit Game"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr(" "), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("S - Toggle Game Sounds ") + tr(m_sounds? "(ON)": "(OFF)"), w/2, y, Qt::AlignHCenter);
   y += m_textOut(p, tr("M - Toggle Music ") + tr(m_music? "(ON)": "(OFF)"), w/2, y, Qt::AlignHCenter);

// The copyright string from the bottom of the page.
   int hy = y;
   y = h - m_txs();
#if 0
//(@) Redundant, over-booked in size, and therefore removed.
   m_setFont(p, asteroid::lfSmall);
   m_textOut(p, AppCopyRight, m_txs(), y, Qt::AlignBottom);
   y -= m_textOut(p, AppDomain, w - m_txs(), y, Qt::AlignRight | Qt::AlignBottom);
#endif

// Additional text (not shown if there is no room).
   m_setFont(p, asteroid::lfMedium);
   th = m_textOut(p, tr(" "), w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

   if (th < y - hy) {
      y = hy + (y - hy - th)/2;
      m_textOut(p, tr("Press SPACE to Play"), w/2, y, Qt::AlignHCenter);
   }
}

// Draw intro screen #2.
void Game::m_drawIntroScreen2() {
   QPainter p(this);
   m_resetScreen(p);

   int w = width();
   int h = height();

   int y = h/8;

// Hi Score.
   m_setFont(p, asteroid::lfHugeBold);
   int th = m_textOut(p, AppName.toUpper(), w/2, y, Qt::AlignHCenter);

   y += 3*th;
   m_setFont(p, asteroid::lfMedium);
   y += m_textOut(p, tr("HIGHEST SCORE : ") + QString::number(mp_engine->hiscore()), w/2, y, Qt::AlignHCenter);

   y += m_textOut(p, tr("LAST SCORE : ") + QString::number(mp_engine->lastScore()), w/2, y, Qt::AlignHCenter);

// The copyright string from the bottom of the page.
   int hy = y;
   y = h - m_txs();
#if 0
//(@) Redundant, over-booked in size, and therefore removed.
   m_setFont(p, asteroid::lfSmall);
   m_textOut(p, AppCopyRight, m_txs(), y, Qt::AlignBottom);
   y -= m_textOut(p, AppDomain, w - m_txs(), y, Qt::AlignRight | Qt::AlignBottom);
#endif

// Additional text (not shown if there's no room).
   m_setFont(p, asteroid::lfMedium);
   th = m_textOut(p, tr(" "), w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

   if (th < y - hy) {
      y = hy + (y - hy - th)/2;
      m_textOut(p, tr("Press SPACE to Play"), w/2, y, Qt::AlignHCenter);
   }
}

// class Game: private slots
// ─────────────────────────
// Internal poller.
// Called repeatedly to update the game state; also responsible for paging through the intro screens.
void Game::m_poll() {
// The media file's pathname.
   const QString path = QCoreApplication::applicationDirPath() + "/media/";

   if (mp_engine->active()) {
   // The game is active, i.e. in play or showing a demo.
   // Update the game state for the next poll, if active.
      if (!m_paused) {
         mp_engine->tick();
      }

   // Move directly to the intro screen at the end of the game,
   // otherwise repaint the updated state coming from the last tick() call.
      if (mp_engine->gameOver()) {
         setGameState(GS_INTRO_0);
      } else {
         update();
      }

      if (m_sounds && mp_engine->playing()) {
         switch (mp_engine->rockExplodeSnd()) {
            case asteroid::otBigRock:
               mp_explodeAudio->setCurrentSource(PhononFile(path, "explode_large.wav")), mp_explodeAudio->play();
            break;
            case asteroid::otMedRock:
               mp_explodeAudio->setCurrentSource(PhononFile(path, "explode_medium.wav")), mp_explodeAudio->play();
            break;
            case asteroid::otSmallRock:
               mp_explodeAudio->setCurrentSource(PhononFile(path, "explode_small.wav")), mp_explodeAudio->play();
            break;
            default: break;
         }

         if (mp_engine->fireSnd())
            mp_fireAudio->setCurrentSource(PhononFile(path, "fire.wav")), mp_fireAudio->play();

         if (!mp_engine->thrustSnd())
            mp_thrustAudio->stop();
         else if (mp_thrustAudio->state() != Phonon::PlayingState && mp_thrustAudio->state() != Phonon::BufferingState)
            mp_thrustAudio->setCurrentSource(PhononFile(path, "thrust.wav")), mp_thrustAudio->play();

         if (mp_engine->alienSnd())
            mp_eventAudio->setCurrentSource(PhononFile(path, "alien.wav")), mp_eventAudio->play();

         if (mp_engine->diedSnd())
            mp_eventAudio->setCurrentSource(PhononFile(path, "die.wav")), mp_eventAudio->play();
      }
   } else if (time(0) >= m_timeMark + INTRO_SCREEN_SEC) {
   // Rotate the intro screens, including any change from the intro to the demo state.
      switch (m_gameState) {
         case GS_INTRO_0: setGameState(GS_INTRO_1); break;
         case GS_INTRO_1: setGameState(GS_INTRO_2); break;
         case GS_INTRO_2: setGameState(GS_DEMO); break;
         default: setGameState(GS_INTRO_0); break;
      }
   }

// Start the music, change the track or stop the music.
   if (m_music && (m_playing != mp_engine->playing() || (mp_musicAudio->state() != Phonon::BufferingState && mp_musicAudio->state() != Phonon::PlayingState))) {
      mp_musicAudio->setCurrentSource(PhononFile(path, mp_engine->playing()? "play.mp3": "intro.mp3"));
      mp_musicAudio->play();
   } else if (!m_music && mp_musicAudio->state() == Phonon::PlayingState) {
      mp_musicAudio->stop();
   }
// Stop any lingering sounds.
   if ((!m_sounds || !mp_engine->playing()) && mp_thrustAudio->state() == Phonon::PlayingState) {
      mp_thrustAudio->stop();
   }
// Hold the last state to detect any change.
   m_playing = mp_engine->playing();
}

// class Game: protected members
// ─────────────────────────────
// The paint event handler: call the appropriate rendering method.
void Game::paintEvent(QPaintEvent *) {
   m_recalcGameArea();

   switch (m_gameState) {
      case GS_INTRO_0: m_drawIntroScreen0(); break;
      case GS_INTRO_1: m_drawIntroScreen1(); break;
      case GS_INTRO_2: m_drawIntroScreen2(); break;
      default: m_drawPlay(); break;
   }
}

// class Game: public members
// ──────────────────────────
// Make a new Game object.
Game::Game(QWidget *parent): QWidget(parent, Qt::Widget) {
   m_paused = false, m_sounds = true, m_music = true, m_playing = false;
   m_soundKeydown = false, m_musicKeydown = false, m_pauseKeydown = false;
   m_backCol = Qt::black, m_foreCol = Qt::white;
   m_gameState = GS_INTRO_0, m_timeMark = time(0);

// Create the media players.
   mp_musicAudio = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource());
   mp_explodeAudio = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   mp_thrustAudio = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   mp_fireAudio = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());
   mp_eventAudio = Phonon::createPlayer(Phonon::GameCategory, Phonon::MediaSource());

// Set up the game engine.
   int w, h;
   mp_engine = new asteroid::Engine();
   mp_engine->getPlayDims(&w, &h);
   m_defaultGameArea = w*h;
   m_recalcGameArea();

// Set up the poll timer.
   _Timer = new QTimer(this);
   connect(_Timer, SIGNAL(timeout()), this, SLOT(m_poll()));
   _Timer->start(DEF_POLL_RATE);
}

// Free the Game object.
Game::~Game() {
   try {
      delete mp_engine;
      delete mp_musicAudio;
      delete mp_explodeAudio;
      delete mp_thrustAudio;
      delete mp_fireAudio;
      delete mp_eventAudio;
   } catch(...) { }
}

// Get/set the game-pause state.
bool Game::isPaused() const {
   return m_paused;
}

void Game::pause(bool p) {
   m_paused = p && isPlaying();
}

// Get/set the game-playing state.
void Game::play(bool p) {
   if (p != isPlaying()) setGameState(p? GS_PLAY: GS_INTRO_0);
}

bool Game::isPlaying() const {
   return gameState() == GS_PLAY;
}

// Get/set the game state.
// Setting can be used to start a game, a demo or to change the intro screen.
// All changes to the game state should go through these procedures.
Game::GameStateType Game::gameState() const {
   return m_gameState;
}

void Game::setGameState(Game::GameStateType gs) {
   if (gs != m_gameState) {
   // Start engine playing, engine demo or kill play/demo.
      if (gs == GS_PLAY) {
         m_paused = false;
         mp_engine->startGame();
      } else if (gs == GS_DEMO) {
         mp_engine->startDemo();
      } else {
         m_paused = false;
         mp_engine->stop();
      }

   // Update the state and hold the time when it was done.
      m_gameState = gs;
      m_timeMark = time(0);
      update();
   }
}

// Get/set the high score.
int Game::hiscore() const {
   return mp_engine->hiscore();
}

void Game::setHiscore(int hs) {
   mp_engine->hiscore(hs);
}

// Get/set the sounding/singing states; foreground/background colors.
// Update the intro pages after any change is made.
bool Game::sounds() const {
   return m_sounds;
}

void Game::setSounds(bool s) {
   if (m_sounds != s) {
      m_sounds = s;

      update();
   }
}

bool Game::music() const {
   return m_music;
}

void Game::setMusic(bool m) {
   if (m_music != m) {
      m_music = m;

      update();
   }
}

QColor Game::foreground() const {
   return m_foreCol;
}

void Game::setForeground(const QColor &c) {
   if (m_foreCol != c) {
      m_foreCol = c;
      update();
   }
}

QColor Game::background() const {
   return m_backCol;
}

void Game::setBackground(const QColor &c) {
   if (m_backCol != c) {
      m_backCol = c;
      update();
   }
}

// Get/set the game level.
// The level ∈ [0,1] determines how fast rocks are created; 0 = easiest, 1 = hardest.
double Game::difficulty() const {
   return mp_engine->difficulty();
}

void Game::setDifficulty(const double &dif) {
   mp_engine->difficulty(dif);
}

// Get/set the game speed; i.e. the polling rate, which is in milliseconds.
int Game::pollRate() const {
   return _Timer->interval();
}

void Game::setPollRate(int ms) {
   _Timer->setInterval(ms);
}

// Handle a key down event; meant to be called from outside this class in response to key events.
// Return true if handled.
bool Game::keyDown(int k) {
   switch (k) {
   // Game control keys down.
      case Qt::Key_K: case Qt::Key_Left: mp_engine->rotate(-1); return true;
      case Qt::Key_L: case Qt::Key_Right: mp_engine->rotate(+1); return true;
      case Qt::Key_A: case Qt::Key_Up: mp_engine->thrust(true); return true;
      case Qt::Key_Control: case Qt::Key_Space: mp_engine->fire(); return true;
#if 0
   // Start game key down: start the game.
      case Qt::Key_Space: play(true); return true;
#endif
   // Stop game key down: stop the game.
      case Qt::Key_Escape: play(false); return true;
   // Sound key down: toggle the sound state.
      case Qt::Key_S:
         if (!m_soundKeydown) {
            setSounds(!m_sounds);
            m_soundKeydown = true;
         }
      return true;
   // Music key down: toggle the music state.
      case Qt::Key_M:
         if (!m_musicKeydown) {
            setMusic(!m_sounds);
            m_musicKeydown = true;
         }
      return true;
   // Pause key down: toggle the pause state.
      case Qt::Key_P:
         if (!m_pauseKeydown) {
            pause(!m_paused);
            m_pauseKeydown = true;
         }
      return true;
   }
   return false;
}

// Handle a key up event; meant to be called from outside this class in response to key events.
// Return true if handled.
bool Game::keyUp(int k) {
   switch (k) {
   // Game control keys up.
      case Qt::Key_K: case Qt::Key_Left: mp_engine->rotate(0); return true;
      case Qt::Key_L: case Qt::Key_Right: mp_engine->rotate(0); return true;
      case Qt::Key_A: case Qt::Key_Up: mp_engine->thrust(false); return true;
      case Qt::Key_Control: case Qt::Key_Space: mp_engine->reload(); return true;
#if 0
   // Start game key up.
      case Qt::Key_Space: return true;
#endif
   // Stop game key up.
      case Qt::Key_Escape: return true;
   // Sound key up.
      case Qt::Key_S: m_soundKeydown = false; return true;
   // Music key up.
      case Qt::Key_M: m_musicKeydown = false; return true;
   // Pause key up.
      case Qt::Key_P: m_pauseKeydown = false; return true;
   }
   return false;
}
