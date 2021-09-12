#ifndef OnceOnlyArena_h
#define OnceOnlyArena_h

// Asteroid Style Game: The game arena.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QMainWindow>

class QMenu;
class QAction;
class QSettings;
class QTimer;
class Game;
class About;

class Arena: public QMainWindow {
Q_OBJECT
private:
   QAction *_NewGameAct, *_EndGameAct, *_EasyAct, *_NormAct, *_HardAct, *_SoundAct, *_MusicAct;
   QSettings *_Settings;
   QTimer *_Timer;
   Game *_Game;
   About *_About;
   void _MainMenu();
   void _GetSettings();
   void _PutSettings();
private slots:
   void _NewGame();
   void _EndGame();
   void _ExitGame();
   void _SetOptions();
   void _ShowWebUrl();
   void _ShowAbout();
   void _UpdateMenu();
protected:
   virtual void keyPressEvent(QKeyEvent *Ev);
   virtual void keyReleaseEvent(QKeyEvent *Ev);
public:
   Arena();
   ~Arena();
};

#endif // OnceOnly
