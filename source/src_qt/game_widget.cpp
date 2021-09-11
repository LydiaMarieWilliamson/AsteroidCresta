//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : main_window.cpp
// DESCRIPTION : Application main window
// COPYRIGHT   : Big Angry Dog (C) 2009
// This file is part of the "Asteroid Cresta" program.
// Asteroid Cresta is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Asteroid Cresta is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with Asteroid Cresta.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------
#include <QtGui>
#include <phonon>
#include <math.h>
#include "game_widget.h"
#include "game_engine.h"
#include "app_version.h"
//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------
const int DEF_POLL_RATE = 45;
const int INTRO_SCREEN_SEC = 8;
const QString SCREEN_FONT_NAME = "serif";
//---------------------------------------------------------------------------
// CLASS GameWidget : PRIVATE MEMBERS
//---------------------------------------------------------------------------
int GameWidget::m_txs() const
{
  // Return scaled text spacer
  return (int)(5.0 * m_scale());
}
//---------------------------------------------------------------------------
double GameWidget::m_scale() const
{
  // Return scaling value based on width.
  int gw;
  mp_engine->getPlayDims(&gw, 0);

  if (gw > 0) return (double)width() / gw;
  else return 1.0;
}
//---------------------------------------------------------------------------
void GameWidget::m_recalcGameArea()
{
  // Recalculate internal gaming area. To be called when parent's size is
  // changed. This alters the aspect ratio of the internal dimensions,
  // while keeping the area approximately constant.
  int w = width();
  int h = height();

  if (h > 0)
  {
    double asr = (double)w / h;
    w = (int)sqrt(asr * m_defaultGameArea);
    h = (int)((double)w / asr);
    mp_engine->setPlayDims(w, h);
  }
}
//---------------------------------------------------------------------------
void GameWidget::m_setFont(QPainter& p, asteroid::LFSize sz, bool bold)
{
  // Set painter font according to size sz
  double ps;
  QFont f = p.font();

  switch(sz)
  {
    case asteroid::lfSmall:
      ps = 10;
      break;
    case asteroid::lfLarge:
      ps = 14;
      break;
    case asteroid::lfHugeBold:
      ps = 16;
      bold = true;
      break;
    default:
      ps = 12;
      break;
  }

  // Scale point size
  ps *= m_scale();
  if (ps < 8) ps = 8;

  // Set bold
  f.setBold(bold);
  f.setPointSizeF(ps);
  p.setFont(f);
}
//---------------------------------------------------------------------------
int GameWidget::m_textOut(QPainter& p, const QString& s, int x, int y,
  Qt::Alignment layout)
{
  // Draw text out at x, y. Returns height of text drawn.
  // Valid layout options include AlignLeft, AlignRight, AlignHCenter,
  // AlignTop, AlignBottom, AlignVCenter and AlignCenter. Others are
  // if ingored. These define how the text is aligned to x and y, rather
  // than with respect to any rectangle. For example, if layout
  // AlignRight,  the right hand edge of the text will be aligned to x.
  QRect tr = p.boundingRect(rect(), 0, s);

  // Horizontal
  if ((layout & Qt::AlignRight) != 0)
  {
    x -= tr.width();
  }
  else
  if ((layout & Qt::AlignHCenter) != 0 || (layout & Qt::AlignCenter) != 0)
  {
    x -= tr.width() / 2;
  }

  // Vertical
  if ((layout & Qt::AlignBottom) != 0)
  {
    y -= tr.height();
  }
  else
  if ((layout & Qt::AlignVCenter) != 0 || (layout & Qt::AlignCenter) != 0)
  {
    y -= tr.height() / 2;
  }

  p.drawText(QRect(x, y, tr.width(), tr.height()), Qt::AlignLeft | Qt::AlignTop, s );
  return tr.height();
}
//---------------------------------------------------------------------------
void GameWidget::m_resetScreen(QPainter& p)
{
  // Render blank painter and setup colors
  p.setFont( QFont(SCREEN_FONT_NAME) );
  p.setPen( QPen(m_foreCol) );
  p.fillRect( rect(), m_backCol );
}
//---------------------------------------------------------------------------
void GameWidget::m_drawPlay()
{
  // Draw play action, including demo phase.
  // This is called during active game play or demo
  // mode to render the game engine ojects.
  QPainter p(this);
  m_resetScreen(p);

  int xo, yo, x, y;
  double sc = m_scale();

  // Loop through game objects
  for(size_t oidx = 0; oidx < mp_engine->objCnt(); ++oidx)
  {
    // Get reference to game object
    asteroid::Obj* obj = mp_engine->objAtIdx(oidx);
    int pc = obj->pointCnt();

    if(!obj->dead())
    {
      if (pc > 0)
      {
        // Render polygon
        xo = (int)(sc * obj->points(0).real());
        yo = (int)(sc * obj->points(0).imag());

        for(int n = 1; n < pc; ++n)
        {
          x = (int)(sc * obj->points(n).real());
          y = (int)(sc * obj->points(n).imag());

          p.drawLine(xo, yo, x, y);

          xo = x;
          yo = y;
        }
      }

      // Render labels
      QString s = tr( obj->caption().c_str() );

      if (!s.isEmpty())
      {
        // Draw new position
        x = (int)(sc * obj->pos.real());
        y = (int)(sc * obj->pos.imag());

        m_setFont(p, obj->fontSize());
        m_textOut(p, s, x, y, Qt::AlignCenter);
      }
    }
  }

  if (m_paused)
  {
    // Show paused
    m_setFont(p, asteroid::lfSmall);
    m_textOut(p, tr("PAUSED"), width() / 2, height() / 2, Qt::AlignCenter);
  }

  // Render scores & lives
  m_setFont(p, asteroid::lfSmall);
  int sh = m_textOut(p, tr("SCORE ") + QString::number(mp_engine->score()),
    m_txs(), m_txs());

  m_textOut(p, tr("LIVES ") + QString::number(mp_engine->lives()),
    m_txs(), m_txs() + sh);

  sh = m_textOut(p, tr("HI SCORE ") + QString::number(mp_engine->hiscore()),
    width() - m_txs(), m_txs(), Qt::AlignRight);

  m_textOut(p, QString(mp_engine->charge(), '|'), m_txs(),
    height() - m_txs(), Qt::AlignBottom);
}
//---------------------------------------------------------------------------
void GameWidget::m_drawIntroScreen0()
{
  // Draw intro screen 0
  QPainter p(this);
  m_resetScreen(p);

  int w = width();
  int h = height();

  int y = h / 8;

  // Titles
  m_setFont(p, asteroid::lfHugeBold);
  y += m_textOut(p, APP_NAME_STR.toUpper(), w/2, y, Qt::AlignHCenter);

  m_setFont(p, asteroid::lfSmall);
  int th = m_textOut(p, APP_COPYRIGHT_STR, w/2, y, Qt::AlignHCenter);

  y += 2 * th;
  m_setFont(p, asteroid::lfMedium);
  y += m_textOut(p, tr("INSERT COIN"), w/2, y, Qt::AlignHCenter);

  // Website string from bottom of page
  int hy = y;
  y = 7 * h / 8;
  m_setFont(p, asteroid::lfMedium);
  y -= m_textOut(p, APP_DOMAIN_STR, w / 2, y, Qt::AlignHCenter | Qt::AlignBottom);

  // Additional text (not shown if no room)
  int tlh = 4 * th;

  if (tlh < y - hy)
  {
    y = hy + (y - hy - tlh) / 2;

    m_setFont(p, asteroid::lfSmall);
    y += m_textOut(p, tr("This game was inspired by Atari Asteroids--a classic from 1979."),
      w/2, y, Qt::AlignHCenter);
    y += m_textOut(p, tr("It is written in C++ with a QT front-end. No warranty."),
      w/2, y, Qt::AlignHCenter);
    y += m_textOut(p, tr("Released under GNU General Public License."),
      w/2, y, Qt::AlignHCenter);
  }

}
//---------------------------------------------------------------------------
void GameWidget::m_drawIntroScreen1()
{
  // Draw intro screen 1
  QPainter p(this);
  m_resetScreen(p);

  int w = width();
  int h = height();

  int y = h / 8;

  // Titles
  m_setFont(p, asteroid::lfHugeBold);
  int th = m_textOut(p, APP_NAME_STR.toUpper(), w/2, y, Qt::AlignHCenter);

  y += 2 * th;
  m_setFont(p, asteroid::lfMedium);
  y += m_textOut(p, tr("CONTROLS"), w/2, y, Qt::AlignHCenter);

  // Keys
  m_setFont(p, asteroid::lfSmall);
  y += m_textOut(p, tr("L ARROW (or K) - Rotate Left"), w/2, y, Qt::AlignHCenter);
  y += m_textOut(p, tr("R ARROW (or L) - Rotate Right"), w/2, y, Qt::AlignHCenter);
  y += m_textOut(p, tr("UP ARROW (or A) - Thrust"), w/2, y, Qt::AlignHCenter);
  y += m_textOut(p, tr("CTRL (or SPACE) - Fire"), w/2, y, Qt::AlignHCenter);
  y += m_textOut(p, tr("P - Pause"), w/2, y, Qt::AlignHCenter);
  y += m_textOut(p, tr("ESC - Quit Game"), w/2, y, Qt::AlignHCenter);
  y += m_textOut(p, tr(" "), w/2, y, Qt::AlignHCenter);

  QString s = tr("(OFF)");
  if (m_sounds) s = tr("(ON)");
  y += m_textOut(p, tr("S - Toggle Game Sounds ") + s, w/2, y, Qt::AlignHCenter);

  QString m = tr("(OFF)");
  if (m_music) m = tr("(ON)");
  y += m_textOut(p, tr("M - Toggle Music ") + m, w/2, y, Qt::AlignHCenter);

  // Copyright string from page bottom
  int hy = y;
  y = h - m_txs();
  m_setFont(p, asteroid::lfSmall);
  m_textOut(p, APP_COPYRIGHT_STR, m_txs(), y, Qt::AlignBottom);
  y -= m_textOut(p, APP_DOMAIN_STR, w - m_txs(), y, Qt::AlignRight | Qt::AlignBottom);

  // Additional text (not shown if no room)
  m_setFont(p, asteroid::lfMedium);
  th = m_textOut(p, tr(" "), w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

  if (th < y - hy)
  {
    y = hy + (y - hy - th) / 2;
    m_textOut(p, tr("Press SPACE to Play"), w/2, y, Qt::AlignHCenter);
  }
}
//---------------------------------------------------------------------------
void GameWidget::m_drawIntroScreen2()
{
  // Draw intro screen 2
  QPainter p(this);
  m_resetScreen(p);

  int w = width();
  int h = height();

  int y = h / 8;

  // Hi Score
  m_setFont(p, asteroid::lfHugeBold);
  int th = m_textOut(p, APP_NAME_STR.toUpper(), w/2, y, Qt::AlignHCenter);

  y += 3 * th;
  m_setFont(p, asteroid::lfMedium);
  y += m_textOut(p, tr("HIGHEST SCORE : ") +
    QString::number(mp_engine->hiscore()), w/2, y, Qt::AlignHCenter);

  y += m_textOut(p, tr("LAST SCORE : ") +
    QString::number(mp_engine->lastScore()), w/2, y, Qt::AlignHCenter);

  // Copyright string from page bottom
  int hy = y;
  y = h - m_txs();
  m_setFont(p, asteroid::lfSmall);
  m_textOut(p, APP_COPYRIGHT_STR, m_txs(), y, Qt::AlignBottom);
  y -= m_textOut(p, APP_DOMAIN_STR, w - m_txs(), y, Qt::AlignRight | Qt::AlignBottom);

  // Additional text (not shown if no room)
  m_setFont(p, asteroid::lfMedium);
  th = m_textOut(p, tr(" "), w/2, y, Qt::AlignHCenter | Qt::AlignBottom);

  if (th < y - hy)
  {
    y = hy + (y - hy - th) / 2;
    m_textOut(p, tr("Press SPACE to Play"), w/2, y, Qt::AlignHCenter);
  }
}
//---------------------------------------------------------------------------
GameWidget::GameKey GameWidget::sm_qkToGk(int qtk)
{
  // Map QT key presss to game control key value
  switch(qtk)
  {
    case Qt::Key_K:
    case Qt::Key_Left: return GK_LEFT;
    case Qt::Key_L:
    case Qt::Key_Right: return GK_RIGHT;
    case Qt::Key_A:
    case Qt::Key_Up: return GK_THRUST;
    case Qt::Key_Control:
    case Qt::Key_Space: return GK_FIRE;
    default: return GK_NONE;
  }
}
//---------------------------------------------------------------------------
// CLASS GameWidget : PRIVATE SLOTS
//---------------------------------------------------------------------------
void GameWidget::m_poll()
{
  // Internal poller. Called repeatedly to update game state. Also
  // responsible for page through the intro screens.
  
  // Media path
  const QString path = QCoreApplication::applicationDirPath() + "/media/";
  
  if (mp_engine->active())
  {
    // The game is active, i.e. in play or showing demo.
    if (!m_paused)
    {
      // Update game state for next poll
      mp_engine->tick();
    }

    if (mp_engine->gameOver())
    {
      // Game or demo has finished, move directly to intro screen.
      setGameState(GS_INTRO_0);
    }
    else
    {
      // Cause repaint, which will reflect updated state after tick() call.
      update();
    }

    if (m_sounds && mp_engine->playing())
    {
      switch( mp_engine->rockExplodeSnd() )
      {
        case asteroid::otBigRock:
          mp_explodeAudio->setCurrentSource(Phonon::MediaSource(path + "explode_large.wav"));
          mp_explodeAudio->play();
          break;
        case asteroid::otMedRock:
          mp_explodeAudio->setCurrentSource(Phonon::MediaSource(path + "explode_medium.wav"));
          mp_explodeAudio->play();
          break;
        case asteroid::otSmallRock:
          mp_explodeAudio->setCurrentSource(Phonon::MediaSource(path + "explode_small.wav"));
          mp_explodeAudio->play();
          break;
        default:
          break;
      }

      if (mp_engine->fireSnd())
      {
        mp_fireAudio->setCurrentSource(Phonon::MediaSource(path + "fire.wav"));
        mp_fireAudio->play();
      }

      if (mp_engine->thrustSnd())
      {
        if (mp_thrustAudio->state() != Phonon::PlayingState &&
          mp_thrustAudio->state() != Phonon::BufferingState)
        {
          mp_thrustAudio->setCurrentSource(Phonon::MediaSource(path + "thrust.wav"));
          mp_thrustAudio->play();
        }
      }
      else
      {
        mp_thrustAudio->stop();
      }

      if (mp_engine->alienSnd())
      {
        mp_eventAudio->setCurrentSource(Phonon::MediaSource(path + "alien.wav"));
        mp_eventAudio->play();
      }

      if (mp_engine->diedSnd())
      {
        mp_eventAudio->setCurrentSource(Phonon::MediaSource(path + "die.wav"));
        mp_eventAudio->play();
      }
    }
  }
  else
  if (time(0) >= m_timeMark + INTRO_SCREEN_SEC)
  {
    // Rotate intro screens, including change from intro to demo state.
    switch(m_gameState)
    {
      case GS_INTRO_0: setGameState(GS_INTRO_1); break;
      case GS_INTRO_1: setGameState(GS_INTRO_2); break;
      case GS_INTRO_2: setGameState(GS_DEMO); break;
      default: setGameState(GS_INTRO_0); break;
    }
  }

  if (m_music && (m_playing != mp_engine->playing() ||
    (mp_musicAudio->state() != Phonon::BufferingState &&
    mp_musicAudio->state() != Phonon::PlayingState)))
  {
    // Play music (or change track)
    if (mp_engine->playing())
    {
      mp_musicAudio->setCurrentSource(Phonon::MediaSource(path + "play.mp3"));
    }
    else
    {
      mp_musicAudio->setCurrentSource(Phonon::MediaSource(path + "intro.mp3"));
    }
      
    mp_musicAudio->play();
  }
  else
  if (!m_music && mp_musicAudio->state() == Phonon::PlayingState)  
  {
    // Stop music
    mp_musicAudio->stop();
  }
  
  // Stop lingering sounds
  if ((!m_sounds || !mp_engine->playing()) &&
    mp_thrustAudio->state() == Phonon::PlayingState)  
  {
    mp_thrustAudio->stop();
  }
  
  // Hold last state to detect change
  m_playing = mp_engine->playing();
}
//---------------------------------------------------------------------------
// CLASS GameWidget : PROTECTED MEMBERS
//---------------------------------------------------------------------------
void GameWidget::paintEvent(QPaintEvent* event)
{
  // Paint event - call appropriate rendering method
  m_recalcGameArea();

  switch(m_gameState)
  {
    case GS_INTRO_0: m_drawIntroScreen0(); break;
    case GS_INTRO_1: m_drawIntroScreen1(); break;
    case GS_INTRO_2: m_drawIntroScreen2(); break;
    default: m_drawPlay(); break;
  }
}
//---------------------------------------------------------------------------
// CLASS GameWidget : PUBLIC MEMBERS
//---------------------------------------------------------------------------
GameWidget::GameWidget(QWidget* parent)
  : QWidget(parent, Qt::Widget)
{
  // Constructor
  m_paused = false;
  m_sounds = true;
  m_music = true;
  m_playing = false;
  m_soundKeydown = false;
  m_musicKeydown = false;
  m_pauseKeydown = false;
  m_backCol = Qt::black;
  m_foreCol = Qt::white;
  m_gameState = GS_INTRO_0;
  m_timeMark = time(0);

  // Create media players
  mp_musicAudio = Phonon::createPlayer(Phonon::MusicCategory,
    Phonon::MediaSource());
  mp_explodeAudio = Phonon::createPlayer(Phonon::GameCategory,
    Phonon::MediaSource());
  mp_thrustAudio = Phonon::createPlayer(Phonon::GameCategory,
    Phonon::MediaSource());
  mp_fireAudio = Phonon::createPlayer(Phonon::GameCategory,
    Phonon::MediaSource());
  mp_eventAudio = Phonon::createPlayer(Phonon::GameCategory,
    Phonon::MediaSource());

  // Setup game engine
  int w, h;
  mp_engine = new asteroid::Engine();
  mp_engine->getPlayDims(&w, &h);
  m_defaultGameArea = w * h;
  m_recalcGameArea();

  // Setup poll timer
  mp_timer = new QTimer(this);
  connect(mp_timer, SIGNAL(timeout()), this, SLOT(m_poll()));
  mp_timer->start(DEF_POLL_RATE);
}
//---------------------------------------------------------------------------
GameWidget::~GameWidget()
{
  // Destructor
  try
  {
    delete mp_engine;
    delete mp_musicAudio;
    delete mp_explodeAudio;
    delete mp_thrustAudio;
    delete mp_fireAudio;
    delete mp_eventAudio;
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
void GameWidget::pause(bool p)
{
  // Pause game
  m_paused = (p && isPlaying());
}
//---------------------------------------------------------------------------
bool GameWidget::isPaused() const
{
  // Accessor - is paused?
  return m_paused;
}
//---------------------------------------------------------------------------
void GameWidget::play(bool p)
{
  // Play game
  if (p != isPlaying())
  {
    if (p)
    {
      setGameState(GS_PLAY);
    }
    else
    {
      setGameState(GS_INTRO_0);
    }
  }
}
//---------------------------------------------------------------------------
bool GameWidget::isPlaying() const
{
  // Acessor - is playing?
  return (gameState() == GS_PLAY);
}
//---------------------------------------------------------------------------
GameWidget::GameStateType GameWidget::gameState() const
{
  // Accessor - game state
  return m_gameState;
}
//---------------------------------------------------------------------------
void GameWidget::setGameState(GameWidget::GameStateType gs)
{
  // Set game state - can be used to start game, a demo or change intro screen.
  // All changes to the game state should be made via this method.
  if (gs != m_gameState)
  {
    if (gs == GS_PLAY)
    {
      // Start engine playing
      m_paused = false;
      mp_engine->startGame();
    }
    else
    if (gs == GS_DEMO)
    {
      // Start engine demo
      mp_engine->startDemo();
    }
    else
    {
      // Kill play or demo
      m_paused = false;
      mp_engine->stop();
    }

    // Update state and hold time it was done
    m_gameState = gs;
    m_timeMark = time(0);
    update();
  }
}
//---------------------------------------------------------------------------
int GameWidget::hiscore() const
{
  // Accessor - hiscore
  return mp_engine->hiscore();
}
//---------------------------------------------------------------------------
void GameWidget::setHiscore(int hs)
{
  // Set hiscore
  mp_engine->hiscore(hs);
}
//---------------------------------------------------------------------------
bool GameWidget::sounds() const
{
  // Accessor - sounds
  return m_sounds;
}
//---------------------------------------------------------------------------
void GameWidget::setSounds(bool s)
{
  // Set sounds
  if (m_sounds != s)
  {
    m_sounds = s;

    // Update intro pages
    update();
  }
}
//---------------------------------------------------------------------------
bool GameWidget::music() const
{
  // Accessor - music
  return m_music;
}
//---------------------------------------------------------------------------
void GameWidget::setMusic(bool m)
{
  // Set sounds
  if (m_music != m)
  {
    m_music = m;

    // Update intro pages
    update();
  }
}
//---------------------------------------------------------------------------
QColor GameWidget::background() const
{
  // Accessor - background color
  return m_backCol;
}
//---------------------------------------------------------------------------
void GameWidget::setBackground(const QColor& c)
{
  // Set background color
  if (m_backCol != c)
  {
    m_backCol = c;
    update();
  }
}
//---------------------------------------------------------------------------
QColor GameWidget::foreground() const
{
  // Accessor - foreground color
  return m_foreCol;
}
//---------------------------------------------------------------------------
void GameWidget::setForeground(const QColor& c)
{
  // Set foreground color
  if (m_foreCol != c)
  {
    m_foreCol = c;
    update();
  }
}
//---------------------------------------------------------------------------
double GameWidget::difficulty() const
{
  // Accessor - difficulty
  // Controls rate at which rocks are created. A value between 0 and 1 where
  // higher values result in harder games.
  return mp_engine->difficulty();
}
//---------------------------------------------------------------------------
void GameWidget::setDifficulty(const double& dif)
{
  // Set difficulty
  // Controls rate at which rocks are created. A value between 0 and 1 where
  // higher values result in harder games.
  mp_engine->difficulty(dif);
}
//---------------------------------------------------------------------------
int GameWidget::pollRate() const
{
  // Accessor - poll rate in ms
  return mp_timer->interval();
}
//---------------------------------------------------------------------------
void GameWidget::setPollRate(int ms)
{
  // Set poll rate in ms
  mp_timer->setInterval(ms);
}
//---------------------------------------------------------------------------
bool GameWidget::keyDown(int k)
{
  // Input a key down event. This method should be called from outside
  // this class in response to key events. Return true if handled.
  GameKey gk = sm_qkToGk(k);

  if (gk != GK_NONE)
  {
    switch(gk)
    {
      case GK_LEFT: mp_engine->rotate(-1); break;
      case GK_RIGHT: mp_engine->rotate(+1); break;
      case GK_THRUST: mp_engine->thrust(true); break;
      case GK_FIRE: mp_engine->fire(); break;
      default: break;
    }

    return true;
  }
  else
  if (k == Qt::Key_Space)
  {
    // Start game
    play(true);
    return true;
  }
  else
  if (k == Qt::Key_Escape)
  {
    // Stop game
    play(false);
    return true;
  }
  else
  if (k == Qt::Key_S)
  {
    // Toggle sound
    if (!m_soundKeydown)
    {
      setSounds( !m_sounds );
      m_soundKeydown = true;
    }

    return true;
  }
  else
  if (k == Qt::Key_M)
  {
    // Toggle music
    if (!m_musicKeydown)
    {
      setMusic( !m_sounds );
      m_musicKeydown = true;
    }

    return true;
  }
  else
  if (k == Qt::Key_P)
  {
    // Toggle music
    if (!m_pauseKeydown)
    {
      pause( !m_paused );
      m_pauseKeydown = true;
    }

   return true;
  }

  return false;
}
//---------------------------------------------------------------------------
bool GameWidget::keyUp(int k)
{
  // Input a key up event. This method should be called from outside
  // this class in response to key events. Return true if handled.
  GameKey gk = sm_qkToGk(k);

  if (gk != GK_NONE)
  {
    switch(gk)
    {
      case GK_LEFT:
      case GK_RIGHT: mp_engine->rotate(0); break;
      case GK_THRUST: mp_engine->thrust(false); break;
      case GK_FIRE: mp_engine->reload(); break;
      default: break;
    }

    return true;
  }
  else
  if (k == Qt::Key_Space || k == Qt::Key_Escape)
  {
    // Handled
    return true;
  }
  else
  if (k == Qt::Key_S)
  {
    // Sound key up
    m_soundKeydown = false;
    return true;
  }
  else
  if (k == Qt::Key_P)
  {
    // Pause key up
    m_pauseKeydown = false;
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
