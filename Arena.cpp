// Asteroid Style Game: The game arena.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QtGui>
#include "Arena.h"
#include "Version.h"
#include "About.h"
#include "Game.h"

// Window constants.
static const int DefXs = 580, DefX = 200;
static const int DefYs = 435, DefY = 200;

// Settings value names.
static const QString WinSizeS = "win_size", WinPosS = "win_pos", WinMaxS = "win_max";
static const QString EasyS = "diff_easy", NormS = "diff_norm", HardS = "diff_hard";
static const QString SoundS = "sounds", MusicS = "music", HiScoreS = "hiscore";

// Game constants.
static const double EasyL = 0.25, NormL = 0.5, HardL = 0.75;

// class Arena: private members
// ────────────────────────────
// Create and set up the main menu.
void Arena::_MainMenu() {
// File.
   QMenu *Menu = menuBar()->addMenu(tr("&File"));
   _NewGameAct = Menu->addAction(tr("&New Game"), this, SLOT(_NewGame()), Qt::Key_Space);
   _EndGameAct = Menu->addAction(tr("&End Game"), this, SLOT(_EndGame()), Qt::Key_Escape), _EndGameAct->setEnabled(false);
   Menu->addSeparator();
   Menu->addAction(tr("&Exit"), this, SLOT(_ExitGame()), Qt::CTRL + Qt::Key_Q);
// Options.
   Menu = menuBar()->addMenu(tr("&Options"));
// Options->Level.
   QActionGroup *Group = new QActionGroup(this);
   _EasyAct = Menu->addAction(tr("&Easy"), this, SLOT(_SetOptions())), _EasyAct->setCheckable(true), Group->addAction(_EasyAct);
   _NormAct = Menu->addAction(tr("&Normal"), this, SLOT(_SetOptions())), _NormAct->setCheckable(true), Group->addAction(_NormAct);
   _HardAct = Menu->addAction(tr("&Hard"), this, SLOT(_SetOptions())), _HardAct->setCheckable(true), Group->addAction(_HardAct);
   Menu->addSeparator();
// Options->Sounds.
   _SoundAct = Menu->addAction(tr("&Sounds"), this, SLOT(_SetOptions()), Qt::Key_S), _SoundAct->setAutoRepeat(false), _SoundAct->setCheckable(true);
   _MusicAct = Menu->addAction(tr("&Music"), this, SLOT(_SetOptions()), Qt::Key_M), _MusicAct->setAutoRepeat(false), _MusicAct->setCheckable(true);
// Help.
   Menu = menuBar()->addMenu(tr("&Help"));
   Menu->addAction(tr("&On the Web"), this, SLOT(_ShowWebUrl()));
   Menu->addAction(tr("&About..."), this, SLOT(_ShowAbout()));
}

// Read the settings.
void Arena::_GetSettings() {
// Maximized or Default.
   if (_Settings->value(WinMaxS, false).toBool())
      move(QPoint(DefX, DefY)), resize(QSize(DefXs, DefYs)), setWindowState(Qt::WindowMaximized);
   else
      move(_Settings->value(WinPosS, QPoint(DefX, DefY)).toPoint()),
      resize(_Settings->value(WinSizeS, QSize(DefXs, DefYs)).toSize());
// Set the menus.
   _EasyAct->setChecked(_Settings->value(EasyS, false).toBool());
   _NormAct->setChecked(_Settings->value(NormS, true).toBool());
   _HardAct->setChecked(_Settings->value(HardS, false).toBool());
   _SoundAct->setChecked(_Settings->value(SoundS, true).toBool());
   _MusicAct->setChecked(_Settings->value(MusicS, true).toBool());
   _Game->SetHiScore(_Settings->value(HiScoreS, 0).toInt());
// Apply the menu settings.
   _SetOptions();
}

// Write all the presets.
void Arena::_PutSettings() {
// Write the window presets.
   _Settings->setValue(WinSizeS, size());
   _Settings->setValue(WinPosS, pos());
   _Settings->setValue(WinMaxS, isMaximized());
// Write the game level presets.
   _Settings->setValue(EasyS, _EasyAct->isChecked());
   _Settings->setValue(NormS, _NormAct->isChecked());
   _Settings->setValue(HardS, _HardAct->isChecked());
// Write the sound/music action presets.
   _Settings->setValue(SoundS, _SoundAct->isChecked());
   _Settings->setValue(MusicS, _MusicAct->isChecked());
// Write the game widget presets.
   _Settings->setValue(HiScoreS, _Game->GetHiScore());
}

// class Arena: private slots
// ──────────────────────────
// Handle the new game, end game and exit events.
void Arena::_NewGame() { _Game->SetPlaying(true); }
void Arena::_EndGame() { _Game->SetPlaying(false); }
void Arena::_ExitGame() { close(); }

// Handle changes to the options state.
void Arena::_SetOptions() {
   _Game->SetLevel(_EasyAct->isChecked()? EasyL: _HardAct->isChecked()? HardL: NormL);
   _Game->SetSounding(_SoundAct->isChecked());
   _Game->SetSinging(_MusicAct->isChecked());
}

// Launch a browser.
void Arena::_ShowWebUrl() { QDesktopServices::openUrl(QUrl(AppUrl)); }

// The about box.
void Arena::_ShowAbout() {
// Create on the first show.
   if (_About == nullptr) _About = new About(this);
   _About->exec();
}

// Timer slot - update the game start/stop menus when the game ends.
void Arena::_UpdateMenu() {
   _NewGameAct->setEnabled(!_Game->GetPlaying()), _EndGameAct->setEnabled(_Game->GetPlaying());
// The sound state.
   _SoundAct->setChecked(_Game->GetSounding());
}

// class Arena: protected members
// ──────────────────────────────
// Handle a key down event.
void Arena::keyPressEvent(QKeyEvent *Ev) {
   if (!_Game->EnKey(Ev->key())) QMainWindow::keyPressEvent(Ev);
}

// Handle a key up event.
void Arena::keyReleaseEvent(QKeyEvent *Ev) {
   if (!_Game->DeKey(Ev->key())) QMainWindow::keyReleaseEvent(Ev);
}

// class Arena: public members
// ───────────────────────────
// Make a new Arena object.
Arena::Arena(): QMainWindow() {
// Set the window type.
   setWindowFlags(Qt::Window), setWindowTitle(AppName), setMinimumSize(300, 225);
// Load the resources.
   setWindowIcon(QIcon(":/Icon32.png"));
// Set up the menus.
   _MainMenu();
// Create the game area.
   _Game = new Game(this), setCentralWidget(_Game);
// Create the settings file.
   _Settings = new QSettings(QSettings::IniFormat, QSettings::SystemScope, AppCompany, AppName, this);
// Set the settings file.
   _GetSettings();
// Apply the options.
   _SetOptions();
// Create a poller to check for game start/end every 1/4 second.
   _Timer = new QTimer(this), connect(_Timer, SIGNAL(timeout()), this, SLOT(_UpdateMenu())), _Timer->start(250);
   _About = nullptr;
}

// Free the Arena object.
Arena::~Arena() {
   try {
   // Update the settings to a file.
      _PutSettings();
   // Delete any objects which do not have this as a parent.
      delete _About;
   } catch(...) { }
}
