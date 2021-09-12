#ifndef OnceOnlyArena_h
#define OnceOnlyArena_h

// Asteroid Style Game: The game arena.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QMainWindow>

class QMenu;
class QAction;
class QSettings;
class QTimer;
class GameWidget;
class About;

class MainWindow: public QMainWindow {
Q_OBJECT
private:
   QAction *mp_newGameAct, *mp_endGameAct, *mp_diffEasyAct, *mp_diffNormAct, *mp_diffHardAct, *mp_soundsAct, *mp_musicAct;
   QSettings *mp_settings;
   QTimer *mp_timer;
   GameWidget *mp_gameWidget;
   About *mp_aboutWindow;
   void m_createMainMenu();
   void m_readSettings();
   void m_writeSettings();
private slots:
   void m_newGame();
   void m_endGame();
   void m_exit();
   void m_setOptions();
   void m_weburl();
   void m_about();
   void m_updateMenu();
protected:
   virtual void keyPressEvent(QKeyEvent *event);
   virtual void keyReleaseEvent(QKeyEvent *event);
public:
   MainWindow();
   ~MainWindow();
};

#endif // OnceOnly
