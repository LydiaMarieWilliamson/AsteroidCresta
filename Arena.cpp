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
#include "Arena.h"
#include "Version.h"
#include "About.h"
#include "Game.h"
//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------

// Window constants
const int DEF_WIDTH = 580;
const int DEF_HEIGHT = 435;
const int DEF_POS_X = 200;
const int DEF_POS_Y = 200;

// Settings value names
const QString SET_WIN_SIZE = "win_size";
const QString SET_WIN_POS = "win_pos";
const QString SET_WIN_MAX = "win_max";
const QString SET_DIFF_EASY = "diff_easy";
const QString SET_DIFF_NORM = "diff_norm";
const QString SET_DIFF_HARD = "diff_hard";
const QString SET_SOUNDS = "sounds";
const QString SET_MUSIC = "music";
const QString SET_HISCORE = "hiscore";

// Game constants
const double DIFF_EASY = 0.25;
const double DIFF_NORM = 0.5;
const double DIFF_HARD = 0.75;

//---------------------------------------------------------------------------
// CLASS MainWindow : PRIVATE MEMBERS
//---------------------------------------------------------------------------
void MainWindow::m_createMainMenu()
{
  // Create & setup main menu

  // File
  QMenu* m_ptr = menuBar()->addMenu( tr("&File") );
  mp_newGameAct = m_ptr->addAction( tr("&New Game"), this, SLOT(m_newGame()), Qt::Key_Space );

  mp_endGameAct = m_ptr->addAction( tr("&End Game"), this, SLOT(m_endGame()), Qt::Key_Escape );
  mp_endGameAct->setEnabled(false);

  m_ptr->addSeparator();
  m_ptr->addAction( tr("&Exit"), this, SLOT(m_exit()), Qt::CTRL + Qt::Key_Q );

  // Options
  m_ptr = menuBar()->addMenu( tr("&Options") );

  // Options->Difficulty
  QActionGroup* diffGroup = new QActionGroup(this);

  mp_diffEasyAct = m_ptr->addAction( tr("&Easy"), this, SLOT(m_setOptions()) );
  mp_diffEasyAct->setCheckable(true);
  diffGroup->addAction(mp_diffEasyAct);

  mp_diffNormAct = m_ptr->addAction( tr("&Normal"), this, SLOT(m_setOptions()) );
  mp_diffNormAct->setCheckable(true);
  diffGroup->addAction(mp_diffNormAct);

  mp_diffHardAct = m_ptr->addAction( tr("&Hard"), this, SLOT(m_setOptions()) );
  mp_diffHardAct->setCheckable(true);
  diffGroup->addAction(mp_diffHardAct);

  m_ptr->addSeparator();

  // Options->Sounds
  mp_soundsAct = m_ptr->addAction( tr("&Sounds"), this, SLOT(m_setOptions()), Qt::Key_S );
  mp_soundsAct->setAutoRepeat(false);
  mp_soundsAct->setCheckable(true);

  mp_musicAct = m_ptr->addAction( tr("&Music"), this, SLOT(m_setOptions()), Qt::Key_M );
  mp_musicAct->setAutoRepeat(false);
  mp_musicAct->setCheckable(true);

  // Help
  m_ptr = menuBar()->addMenu( tr("&Help") );
  m_ptr->addAction( tr("&On the Web"), this, SLOT(m_weburl()) );
  m_ptr->addAction( tr("&About..."), this, SLOT(m_about()) );
}
//---------------------------------------------------------------------------
void MainWindow::m_readSettings()
{
  // Read settings
  if (mp_settings->value(SET_WIN_MAX, false).toBool())
  {
    // Maximized
    move( QPoint(DEF_POS_X, DEF_POS_Y) );
    resize( QSize(DEF_WIDTH, DEF_HEIGHT) );
    setWindowState(Qt::WindowMaximized);
  }
  else
  {
    // Default
    move( mp_settings->value(SET_WIN_POS, QPoint(DEF_POS_X, DEF_POS_Y)).toPoint() );
    resize( mp_settings->value(SET_WIN_SIZE, QSize(DEF_WIDTH, DEF_HEIGHT)).toSize() );
  }

  // Set menus
  mp_diffEasyAct->setChecked( mp_settings->value(SET_DIFF_EASY, false).toBool() );
  mp_diffNormAct->setChecked( mp_settings->value(SET_DIFF_NORM, true).toBool() );
  mp_diffHardAct->setChecked( mp_settings->value(SET_DIFF_HARD, false).toBool() );
  mp_soundsAct->setChecked( mp_settings->value(SET_SOUNDS, true).toBool() );
  mp_musicAct->setChecked( mp_settings->value(SET_MUSIC, true).toBool() );

  mp_gameWidget->setHiscore( mp_settings->value(SET_HISCORE, 0).toInt() );

  // Apply menu settings
  m_setOptions();
}
//---------------------------------------------------------------------------
void MainWindow::m_writeSettings()
{
  // Write settings
  mp_settings->setValue( SET_WIN_SIZE, size() );
  mp_settings->setValue( SET_WIN_POS, pos() );
  mp_settings->setValue( SET_WIN_MAX, isMaximized() );

  mp_settings->setValue( SET_DIFF_EASY, mp_diffEasyAct->isChecked() );
  mp_settings->setValue( SET_DIFF_NORM, mp_diffNormAct->isChecked() );
  mp_settings->setValue( SET_DIFF_HARD, mp_diffHardAct->isChecked() );

  mp_settings->setValue( SET_SOUNDS, mp_soundsAct->isChecked() );
  mp_settings->setValue( SET_MUSIC, mp_musicAct->isChecked() );

  mp_settings->setValue( SET_HISCORE, mp_gameWidget->hiscore() );
}
//---------------------------------------------------------------------------
// CLASS MainWindow : PRIVATE SLOTS
//---------------------------------------------------------------------------
void MainWindow::m_newGame()
{
  // New game event
  mp_gameWidget->play(true);
}
//---------------------------------------------------------------------------
void MainWindow::m_endGame()
{
  // End game event
  mp_gameWidget->play(false);
}
//---------------------------------------------------------------------------
void MainWindow::m_exit()
{
  // Exit event
  close();
}
//---------------------------------------------------------------------------
void MainWindow::m_setOptions()
{
  // Options state changed
  if (mp_diffEasyAct->isChecked())
  {
    mp_gameWidget->setDifficulty(DIFF_EASY);
  }
  else
  if (mp_diffEasyAct->isChecked())
  {
    mp_gameWidget->setDifficulty(DIFF_HARD);
  }
  else
  {
    mp_gameWidget->setDifficulty(DIFF_NORM);
  }

  mp_gameWidget->setSounds(mp_soundsAct->isChecked());
  mp_gameWidget->setMusic(mp_musicAct->isChecked());
}
//---------------------------------------------------------------------------
void MainWindow::m_weburl()
{
  // Launch browser
  QDesktopServices::openUrl( QUrl(APP_URL_STR) );
}
//---------------------------------------------------------------------------
void MainWindow::m_about()
{
  // About box
  if (mp_aboutWindow == 0)
  {
    // Create on first show
    mp_aboutWindow = new AboutWindow(this);
  }

  mp_aboutWindow->exec();
}
//---------------------------------------------------------------------------
void MainWindow::m_updateMenu()
{
  // Timer slot - update game start/stop menus when game ends.
  mp_newGameAct->setEnabled( !mp_gameWidget->isPlaying() );
  mp_endGameAct->setEnabled( mp_gameWidget->isPlaying() );

  // Sound state
  mp_soundsAct->setChecked( mp_gameWidget->sounds() );
}
//---------------------------------------------------------------------------
// CLASS MainWindow : PROTECTED MEMBERS
//---------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent* event)
{
  // Key down event
  if (!mp_gameWidget->keyDown(event->key()))
  {
    QMainWindow::keyPressEvent(event);
  }
}
//---------------------------------------------------------------------------
void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
  // Key up event
  if (!mp_gameWidget->keyUp(event->key()))
  {
    QMainWindow::keyReleaseEvent(event);
  }
}
//---------------------------------------------------------------------------
// CLASS MainWindow : PUBLIC MEMBERS
//---------------------------------------------------------------------------
MainWindow::MainWindow()
  : QMainWindow()
{
  // Set window type
  setWindowFlags(Qt::Window);
  setWindowTitle( APP_NAME_STR );
  setMinimumSize(300, 225);

  // Load resources
  setWindowIcon(QIcon(":/icon32.png"));

  // Setup menus
  m_createMainMenu();

  // Create game area
  mp_gameWidget = new GameWidget(this);
  setCentralWidget(mp_gameWidget);

  // Create settings file
  mp_settings = new QSettings ( QSettings::IniFormat, QSettings::SystemScope,
    APP_COMPANY_STR, APP_NAME_STR, this );

  // Set settings file
  m_readSettings();

  // Apply options
  m_setOptions();

  // Create poller to check for game start/end every 0.25 seconds
  mp_timer = new QTimer(this);
  connect(mp_timer, SIGNAL(timeout()), this, SLOT(m_updateMenu()));
  mp_timer->start(250);

  mp_aboutWindow = 0;
}
//---------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  // Destructor
  try
  {
    // Update settings to file
    m_writeSettings();

    // Delete objects which do not have this as a parent
    delete mp_aboutWindow;
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------
