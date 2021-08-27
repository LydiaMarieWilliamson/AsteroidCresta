//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : main_window.h
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
// HEADER GUARD
//---------------------------------------------------------------------------
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------
#include <QMainWindow>
//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------

// Forward declaration
class QMenu;
class QAction;
class QSettings;
class QTimer;
class GameWidget;
class AboutWindow;

//---------------------------------------------------------------------------
// CLASS MainWindow
//---------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
  Q_OBJECT
private:
  // Private members
  QAction* mp_newGameAct;
  QAction* mp_endGameAct;
  QAction* mp_diffEasyAct;
  QAction* mp_diffNormAct;
  QAction* mp_diffHardAct;
  QAction* mp_soundsAct;
  QAction* mp_musicAct;
  QSettings* mp_settings;
  QTimer* mp_timer;
  GameWidget* mp_gameWidget;
  AboutWindow* mp_aboutWindow;
  
  void m_createMainMenu();
  void m_readSettings();
  void m_writeSettings();
private slots:
  // Private slots
  void m_newGame();
  void m_endGame();
  void m_exit();
  void m_setOptions();
  void m_weburl();
  void m_about();
  void m_updateMenu();
protected:
  // Protected members
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);
public:
  // Construction
  MainWindow();
  ~MainWindow();

  // Public members
};
//---------------------------------------------------------------------------
#endif // HEADER GUARD
//---------------------------------------------------------------------------
