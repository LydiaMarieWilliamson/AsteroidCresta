// Asteroid Style Game: The game arena.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QtGui>
#include "Arena.h"
#include "Version.h"
#include "About.h"
#include "Game.h"

// Window constants.
const int DEF_WIDTH = 580;
const int DEF_POS_X = 200;
const int DEF_HEIGHT = 435;
const int DEF_POS_Y = 200;

// Settings value names.
const QString SET_WIN_SIZE = "win_size";
const QString SET_WIN_POS = "win_pos";
const QString SET_WIN_MAX = "win_max";
const QString SET_DIFF_EASY = "diff_easy";
const QString SET_DIFF_NORM = "diff_norm";
const QString SET_DIFF_HARD = "diff_hard";
const QString SET_SOUNDS = "sounds";
const QString SET_MUSIC = "music";
const QString SET_HISCORE = "hiscore";

// Game constants.
const double DIFF_EASY = 0.25;
const double DIFF_NORM = 0.5;
const double DIFF_HARD = 0.75;

// class MainWindow: private members
// Create and set up the main menu.
void MainWindow::m_createMainMenu() {
// File.
   QMenu *m_ptr = menuBar()->addMenu(tr("&File"));
   mp_newGameAct = m_ptr->addAction(tr("&New Game"), this, SLOT(m_newGame()), Qt::Key_Space);

   mp_endGameAct = m_ptr->addAction(tr("&End Game"), this, SLOT(m_endGame()), Qt::Key_Escape);
   mp_endGameAct->setEnabled(false);

   m_ptr->addSeparator();
   m_ptr->addAction(tr("&Exit"), this, SLOT(m_exit()), Qt::CTRL + Qt::Key_Q);

// Options.
   m_ptr = menuBar()->addMenu(tr("&Options"));

// Options->Difficulty.
   QActionGroup *diffGroup = new QActionGroup(this);

   mp_diffEasyAct = m_ptr->addAction(tr("&Easy"), this, SLOT(m_setOptions()));
   mp_diffEasyAct->setCheckable(true);
   diffGroup->addAction(mp_diffEasyAct);

   mp_diffNormAct = m_ptr->addAction(tr("&Normal"), this, SLOT(m_setOptions()));
   mp_diffNormAct->setCheckable(true);
   diffGroup->addAction(mp_diffNormAct);

   mp_diffHardAct = m_ptr->addAction(tr("&Hard"), this, SLOT(m_setOptions()));
   mp_diffHardAct->setCheckable(true);
   diffGroup->addAction(mp_diffHardAct);

   m_ptr->addSeparator();

// Options->Sounds.
   mp_soundsAct = m_ptr->addAction(tr("&Sounds"), this, SLOT(m_setOptions()), Qt::Key_S);
   mp_soundsAct->setAutoRepeat(false);
   mp_soundsAct->setCheckable(true);

   mp_musicAct = m_ptr->addAction(tr("&Music"), this, SLOT(m_setOptions()), Qt::Key_M);
   mp_musicAct->setAutoRepeat(false);
   mp_musicAct->setCheckable(true);

// Help.
   m_ptr = menuBar()->addMenu(tr("&Help"));
   m_ptr->addAction(tr("&On the Web"), this, SLOT(m_weburl()));
   m_ptr->addAction(tr("&About..."), this, SLOT(m_about()));
}

// Read the settings.
void MainWindow::m_readSettings() {
// Maximized or Default.
   if (mp_settings->value(SET_WIN_MAX, false).toBool()) {
      move(QPoint(DEF_POS_X, DEF_POS_Y));
      resize(QSize(DEF_WIDTH, DEF_HEIGHT));
      setWindowState(Qt::WindowMaximized);
   } else {
      move(mp_settings->value(SET_WIN_POS, QPoint(DEF_POS_X, DEF_POS_Y)).toPoint());
      resize(mp_settings->value(SET_WIN_SIZE, QSize(DEF_WIDTH, DEF_HEIGHT)).toSize());
   }

// Set the menus.
   mp_diffEasyAct->setChecked(mp_settings->value(SET_DIFF_EASY, false).toBool());
   mp_diffNormAct->setChecked(mp_settings->value(SET_DIFF_NORM, true).toBool());
   mp_diffHardAct->setChecked(mp_settings->value(SET_DIFF_HARD, false).toBool());
   mp_soundsAct->setChecked(mp_settings->value(SET_SOUNDS, true).toBool());
   mp_musicAct->setChecked(mp_settings->value(SET_MUSIC, true).toBool());

   mp_gameWidget->setHiscore(mp_settings->value(SET_HISCORE, 0).toInt());

// Apply the menu settings.
   m_setOptions();
}

// Write all the presets.
void MainWindow::m_writeSettings() {
// Write the window presets.
   mp_settings->setValue(SET_WIN_SIZE, size());
   mp_settings->setValue(SET_WIN_POS, pos());
   mp_settings->setValue(SET_WIN_MAX, isMaximized());

// Write the game level presets.
   mp_settings->setValue(SET_DIFF_EASY, mp_diffEasyAct->isChecked());
   mp_settings->setValue(SET_DIFF_NORM, mp_diffNormAct->isChecked());
   mp_settings->setValue(SET_DIFF_HARD, mp_diffHardAct->isChecked());

// Write the sound/music presets.
   mp_settings->setValue(SET_SOUNDS, mp_soundsAct->isChecked());
   mp_settings->setValue(SET_MUSIC, mp_musicAct->isChecked());

// Write the game widget presets.
   mp_settings->setValue(SET_HISCORE, mp_gameWidget->hiscore());
}

// class MainWindow: private slots
// ───────────────────────────────
// Handle the new game, end game and exit events.
void MainWindow::m_newGame() {
   mp_gameWidget->play(true);
}

void MainWindow::m_endGame() {
   mp_gameWidget->play(false);
}

void MainWindow::m_exit() {
   close();
}

// Handle changes to the options state.
void MainWindow::m_setOptions() {
   if (mp_diffEasyAct->isChecked()) {
      mp_gameWidget->setDifficulty(DIFF_EASY);
   } else if (mp_diffEasyAct->isChecked()) {
      mp_gameWidget->setDifficulty(DIFF_HARD);
   } else {
      mp_gameWidget->setDifficulty(DIFF_NORM);
   }

   mp_gameWidget->setSounds(mp_soundsAct->isChecked());
   mp_gameWidget->setMusic(mp_musicAct->isChecked());
}

// Launch a browser.
void MainWindow::m_weburl() {
   QDesktopServices::openUrl(QUrl(APP_URL_STR));
}

// The about box.
void MainWindow::m_about() {
// Create on the first show.
   if (mp_aboutWindow == 0) {
      mp_aboutWindow = new AboutWindow(this);
   }

   mp_aboutWindow->exec();
}

// Timer slot - update the game start/stop menus when the game ends.
void MainWindow::m_updateMenu() {
   mp_newGameAct->setEnabled(!mp_gameWidget->isPlaying());
   mp_endGameAct->setEnabled(mp_gameWidget->isPlaying());

// The sound state.
   mp_soundsAct->setChecked(mp_gameWidget->sounds());
}

// class MainWindow: protected members
// ───────────────────────────────────
// Handle a key down event.
void MainWindow::keyPressEvent(QKeyEvent *event) {
   if (!mp_gameWidget->keyDown(event->key())) {
      QMainWindow::keyPressEvent(event);
   }
}

// Handle a key up event.
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
   if (!mp_gameWidget->keyUp(event->key())) {
      QMainWindow::keyReleaseEvent(event);
   }
}

// class MainWindow: public members
// ────────────────────────────────
// Make a new MainWindow object.
MainWindow::MainWindow(): QMainWindow() {
// Set the window type.
   setWindowFlags(Qt::Window);
   setWindowTitle(APP_NAME_STR);
   setMinimumSize(300, 225);

// Load the resources.
   setWindowIcon(QIcon(":/Icon32.png"));

// Set up the menus.
   m_createMainMenu();

// Create the game area.
   mp_gameWidget = new GameWidget(this);
   setCentralWidget(mp_gameWidget);

// Create the settings file.
   mp_settings = new QSettings(QSettings::IniFormat, QSettings::SystemScope, APP_COMPANY_STR, APP_NAME_STR, this);

// Set the settings file.
   m_readSettings();

// Apply the options.
   m_setOptions();

// Create a poller to check for game start/end every 1/4 second.
   mp_timer = new QTimer(this);
   connect(mp_timer, SIGNAL(timeout()), this, SLOT(m_updateMenu()));
   mp_timer->start(250);

   mp_aboutWindow = 0;
}

// Free the MainWindow object.
MainWindow::~MainWindow() {
   try {
   // Update settings to a file.
      m_writeSettings();

   // Delete any objects which do not have this as a parent.
      delete mp_aboutWindow;
   } catch(...) { }
}
