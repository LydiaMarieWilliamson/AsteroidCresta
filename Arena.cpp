// Asteroid Style Game: The game arena.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QtGui>
#include "Arena.h"
#include "Version.h"
#include "About.h"
#include "Game.h"

// Window constants.
static const int DEF_WIDTH = 580, DEF_POS_X = 200;
static const int DEF_HEIGHT = 435, DEF_POS_Y = 200;

// Settings value names.
static const QString SET_WIN_SIZE = "win_size", SET_WIN_POS = "win_pos", SET_WIN_MAX = "win_max";
static const QString SET_DIFF_EASY = "diff_easy", SET_DIFF_NORM = "diff_norm", SET_DIFF_HARD = "diff_hard";
static const QString SET_SOUNDS = "sounds", SET_MUSIC = "music", SET_HISCORE = "hiscore";

// Game constants.
static const double DIFF_EASY = 0.25, DIFF_NORM = 0.5, DIFF_HARD = 0.75;

// class Arena: private members
// ────────────────────────────
// Create and set up the main menu.
void Arena::_MainMenu() {
// File.
   QMenu *m_ptr = menuBar()->addMenu(tr("&File"));
   _NewGameAct = m_ptr->addAction(tr("&New Game"), this, SLOT(_NewGame()), Qt::Key_Space);

   _EndGameAct = m_ptr->addAction(tr("&End Game"), this, SLOT(_EndGame()), Qt::Key_Escape);
   _EndGameAct->setEnabled(false);

   m_ptr->addSeparator();
   m_ptr->addAction(tr("&Exit"), this, SLOT(_ExitGame()), Qt::CTRL + Qt::Key_Q);

// Options.
   m_ptr = menuBar()->addMenu(tr("&Options"));

// Options->Difficulty.
   QActionGroup *diffGroup = new QActionGroup(this);

   _EasyAct = m_ptr->addAction(tr("&Easy"), this, SLOT(_SetOptions()));
   _EasyAct->setCheckable(true);
   diffGroup->addAction(_EasyAct);

   _NormAct = m_ptr->addAction(tr("&Normal"), this, SLOT(_SetOptions()));
   _NormAct->setCheckable(true);
   diffGroup->addAction(_NormAct);

   _HardAct = m_ptr->addAction(tr("&Hard"), this, SLOT(_SetOptions()));
   _HardAct->setCheckable(true);
   diffGroup->addAction(_HardAct);

   m_ptr->addSeparator();

// Options->Sounds.
   _SoundAct = m_ptr->addAction(tr("&Sounds"), this, SLOT(_SetOptions()), Qt::Key_S);
   _SoundAct->setAutoRepeat(false);
   _SoundAct->setCheckable(true);

   _MusicAct = m_ptr->addAction(tr("&Music"), this, SLOT(_SetOptions()), Qt::Key_M);
   _MusicAct->setAutoRepeat(false);
   _MusicAct->setCheckable(true);

// Help.
   m_ptr = menuBar()->addMenu(tr("&Help"));
   m_ptr->addAction(tr("&On the Web"), this, SLOT(_ShowWebUrl()));
   m_ptr->addAction(tr("&About..."), this, SLOT(_ShowAbout()));
}

// Read the settings.
void Arena::_GetSettings() {
// Maximized or Default.
   if (_Settings->value(SET_WIN_MAX, false).toBool()) {
      move(QPoint(DEF_POS_X, DEF_POS_Y));
      resize(QSize(DEF_WIDTH, DEF_HEIGHT));
      setWindowState(Qt::WindowMaximized);
   } else {
      move(_Settings->value(SET_WIN_POS, QPoint(DEF_POS_X, DEF_POS_Y)).toPoint());
      resize(_Settings->value(SET_WIN_SIZE, QSize(DEF_WIDTH, DEF_HEIGHT)).toSize());
   }

// Set the menus.
   _EasyAct->setChecked(_Settings->value(SET_DIFF_EASY, false).toBool());
   _NormAct->setChecked(_Settings->value(SET_DIFF_NORM, true).toBool());
   _HardAct->setChecked(_Settings->value(SET_DIFF_HARD, false).toBool());
   _SoundAct->setChecked(_Settings->value(SET_SOUNDS, true).toBool());
   _MusicAct->setChecked(_Settings->value(SET_MUSIC, true).toBool());

   _Game->SetHiScore(_Settings->value(SET_HISCORE, 0).toInt());

// Apply the menu settings.
   _SetOptions();
}

// Write all the presets.
void Arena::_PutSettings() {
// Write the window presets.
   _Settings->setValue(SET_WIN_SIZE, size());
   _Settings->setValue(SET_WIN_POS, pos());
   _Settings->setValue(SET_WIN_MAX, isMaximized());

// Write the game level presets.
   _Settings->setValue(SET_DIFF_EASY, _EasyAct->isChecked());
   _Settings->setValue(SET_DIFF_NORM, _NormAct->isChecked());
   _Settings->setValue(SET_DIFF_HARD, _HardAct->isChecked());

// Write the sound/music action presets.
   _Settings->setValue(SET_SOUNDS, _SoundAct->isChecked());
   _Settings->setValue(SET_MUSIC, _MusicAct->isChecked());

// Write the game widget presets.
   _Settings->setValue(SET_HISCORE, _Game->GetHiScore());
}

// class Arena: private slots
// ──────────────────────────
// Handle the new game, end game and exit events.
void Arena::_NewGame() {
   _Game->SetPlaying(true);
}

void Arena::_EndGame() {
   _Game->SetPlaying(false);
}

void Arena::_ExitGame() {
   close();
}

// Handle changes to the options state.
void Arena::_SetOptions() {
   _Game->SetLevel(_EasyAct->isChecked()? DIFF_EASY: _EasyAct->isChecked()? DIFF_HARD: DIFF_NORM);
   _Game->SetSounding(_SoundAct->isChecked());
   _Game->SetSinging(_MusicAct->isChecked());
}

// Launch a browser.
void Arena::_ShowWebUrl() {
   QDesktopServices::openUrl(QUrl(AppUrl));
}

// The about box.
void Arena::_ShowAbout() {
// Create on the first show.
   if (_About == nullptr) {
      _About = new About(this);
   }

   _About->exec();
}

// Timer slot - update the game start/stop menus when the game ends.
void Arena::_UpdateMenu() {
   _NewGameAct->setEnabled(!_Game->GetPlaying());
   _EndGameAct->setEnabled(_Game->GetPlaying());

// The sound state.
   _SoundAct->setChecked(_Game->GetSounding());
}

// class Arena: protected members
// ──────────────────────────────
// Handle a key down event.
void Arena::keyPressEvent(QKeyEvent *event) {
   if (!_Game->EnKey(event->key())) {
      QMainWindow::keyPressEvent(event);
   }
}

// Handle a key up event.
void Arena::keyReleaseEvent(QKeyEvent *event) {
   if (!_Game->DeKey(event->key())) {
      QMainWindow::keyReleaseEvent(event);
   }
}

// class Arena: public members
// ───────────────────────────
// Make a new Arena object.
Arena::Arena(): QMainWindow() {
// Set the window type.
   setWindowFlags(Qt::Window);
   setWindowTitle(AppName);
   setMinimumSize(300, 225);

// Load the resources.
   setWindowIcon(QIcon(":/Icon32.png"));

// Set up the menus.
   _MainMenu();

// Create the game area.
   _Game = new Game(this);
   setCentralWidget(_Game);

// Create the settings file.
   _Settings = new QSettings(QSettings::IniFormat, QSettings::SystemScope, AppCompany, AppName, this);

// Set the settings file.
   _GetSettings();

// Apply the options.
   _SetOptions();

// Create a poller to check for game start/end every 1/4 second.
   _Timer = new QTimer(this);
   connect(_Timer, SIGNAL(timeout()), this, SLOT(_UpdateMenu()));
   _Timer->start(250);

   _About = nullptr;
}

// Free the Arena object.
Arena::~Arena() {
   try {
   // Update settings to a file.
      _PutSettings();

   // Delete any objects which do not have this as a parent.
      delete _About;
   } catch(...) { }
}
