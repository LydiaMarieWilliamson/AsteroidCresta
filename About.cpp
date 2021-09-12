// Asteroid Style Game: The about window.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QtGui>
#include "About.h"
#include "Version.h"

// class GameWidget: protected members
// ───────────────────────────────────
void AboutWindow::paintEvent(QPaintEvent *) {
// Re-paint on displayWidget.
   QPainter painter(this);
   painter.drawPixmap(0, 0, *mp_artwork);

   painter.setPen(QPen(Qt::white));

   QFont f("serif");
   f.setPointSizeF(16);
   f.setBold(true);
   painter.setFont(f);

   int x = m_spacer;
   int y = m_spacer;

   QRect sr = rect();
   int sw = mp_artwork->width() - 2*m_spacer;

// Write the text.
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

   painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop, tr("Version: ") + APP_VERSION_STR);
   y += r.height();

// Up from the bottom of the image.
   y = mp_artwork->height() - r.height();
   painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop, tr("No Warranty. GNU General Public License."));
   y -= r.height();

   painter.drawText(x, y, sw, r.height(), Qt::AlignLeft | Qt::AlignTop, APP_DOMAIN_STR);
}

// class GameWidget: public members
// ────────────────────────────────
// Make a new AboutWindow object.
AboutWindow::AboutWindow(QWidget *parent): QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint) {
   setWindowTitle(tr("About..."));

// Create the display area.
   mp_artwork = new QPixmap(":/Artwork.png", 0);

// Create a button box.
   QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
   connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

// Set up the layout grid.
// Note: we add a spacer at to create an area in which to draw the image and zero the spacing between items.
// However, there still exists a contentsMargin spacing around the layout itself.
// In order to leave a margin around the button bar, we size the draw area so as to include the margin.
// Because we will draw the image at the top left of client window, no margin will be shown around the image
// and the top spacer area will be used to create a spacer between the image and button bar instead.
   QGridLayout *layout = new QGridLayout(this);
   layout->setHorizontalSpacing(0);
   layout->setVerticalSpacing(0);

   int cl, ct, cr, cb;
   layout->getContentsMargins(&cl, &ct, &cr, &cb);
   QSpacerItem *displayArea = new QSpacerItem(mp_artwork->width() - cl - cr, mp_artwork->height());

   layout->addItem(displayArea, 0, 0);
   layout->addWidget(buttonBox, 1, 0);

// Hold the contents spacing.
   m_spacer = cl;
}

// Free the AboutWindow object.
AboutWindow::~AboutWindow() {
   try {
      delete mp_artwork;
   } catch(...) { }
}
