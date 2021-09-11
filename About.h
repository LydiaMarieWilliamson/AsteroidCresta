//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : about window.h
// DESCRIPTION : About window
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
#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------
#include <QDialog>
//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------

// Forward declarations
class QPixmap;
//---------------------------------------------------------------------------
// CLASS MainWindow
//---------------------------------------------------------------------------
class AboutWindow : public QDialog
{
  Q_OBJECT
public:
private:
  // Private members
  QPixmap* mp_artwork;
  int m_spacer;
private slots:
  // Private slots
protected:
  // Protected members
  virtual void paintEvent(QPaintEvent* event);
public:

  // Construction
  AboutWindow(QWidget* parent);
  ~AboutWindow();

  // Public members
};
//---------------------------------------------------------------------------
#endif // HEADER GUARD
//---------------------------------------------------------------------------
