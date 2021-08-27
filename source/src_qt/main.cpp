//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : main.cpp
// DESCRIPTION : C main function
// COPYRIGHT   : Andy Thomas (C) 2009
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
#include <QApplication>
#include "main_window.h"
//---------------------------------------------------------------------------
// MAIN FUNCTION
//---------------------------------------------------------------------------
int main(int argc, char **argv)
{
  try
  {
    QApplication app(argc, argv);
    QPixmap pixmap(":/artwork.bmp");
  
    MainWindow main_win;
    main_win.show();

    return app.exec();
  }
  catch(...)
  {
    return -1;
  }
}
//---------------------------------------------------------------------------
