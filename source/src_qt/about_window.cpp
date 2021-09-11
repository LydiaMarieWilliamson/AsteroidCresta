//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : about window.cpp
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
// INCLUDES
//---------------------------------------------------------------------------
#include <QtGui>
#include "about_window.h"
#include "app_version.h"
//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CLASS GameWidget : PRIVATE MEMBERS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CLASS GameWidget : PRIVATE SLOTS
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CLASS GameWidget : PROTECTED MEMBERS
//---------------------------------------------------------------------------
void AboutWindow::paintEvent(QPaintEvent* event)
{
  // Re-paint on displayWidget
  QPainter painter(this);
  painter.drawPixmap(0, 0, *mp_artwork);

  painter.setPen( QPen(Qt::white) );  
  
  QFont f("serif");
  f.setPointSizeF(16);
  f.setBold(true);
  painter.setFont(f);

  int x = m_spacer;
  int y = m_spacer;

  QRect sr = rect();
  int sw = mp_artwork->width() - 2 * m_spacer;

  // Write text
  QString s = APP_NAME_STR;
  QRect r = painter.boundingRect(sr, 0, s);
  painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop, s);
  y += r.height();

  f.setPointSizeF(10);
  f.setBold(false);
  painter.setFont(f);
  
  s = APP_COPYRIGHT_STR;
  r = painter.boundingRect(sr, 0, s);
  painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop, s);
  y += r.height();

  painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop,
    tr("Version: ") + APP_VERSION_STR);
  y += r.height();

  // Up from bottom of image
  y = mp_artwork->height() - r.height();
  painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop,
    tr("No Warranty. GNU General Public License."));
  y -= r.height();

  painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop,
    APP_DOMAIN_STR);
}
//---------------------------------------------------------------------------
// CLASS GameWidget : PUBLIC MEMBERS
//---------------------------------------------------------------------------
AboutWindow::AboutWindow(QWidget* parent)
  : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
  // Constructor
  setWindowTitle(tr("About..."));

  // Create display area
  mp_artwork = new QPixmap(":/artwork.png", 0);

  // Create button box
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok,
    Qt::Horizontal, this);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

  // Setup layout
  // Note. We add a spacer at to create an area in which to draw the image and
  // zero the spacing between items. However, there still exists a
  // contentsMargin spacing around the layout itself. In order to leave a margin
  // around the button bar, we size the draw area so as to include the margin.
  // Because we will draw the image at the top left of client window, no margin
  // will be shown around the image and the top spacer area will be used to
  // create a spacer between the image and button bar instead.
  QGridLayout* layout = new QGridLayout(this);
  layout->setHorizontalSpacing(0);
  layout->setVerticalSpacing(0);
  
  int cl, ct, cr, cb;
  layout->getContentsMargins(&cl, &ct, &cr, &cb);
  QSpacerItem* displayArea = new QSpacerItem( mp_artwork->width() - cl - cr,
    mp_artwork->height() );
  
  layout->addItem(displayArea, 0, 0);
  layout->addWidget( buttonBox, 1, 0 );
  
  // Hold contents spacing
  m_spacer = cl;
}
//---------------------------------------------------------------------------
AboutWindow::~AboutWindow()
{
  // Destructor
  try
  {
    delete mp_artwork;
  }
  catch(...)
  {
  }
}
//---------------------------------------------------------------------------