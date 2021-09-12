// Asteroid Style Game: The about window.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QtGui>
#include "About.h"
#include "Version.h"

// class About: protected members
// ──────────────────────────────
// Re-paint on displayWidget.
void About::paintEvent(QPaintEvent * /*Ev*/) {
   QPainter Pnt(this); Pnt.drawPixmap(0, 0, *_ArtWork), Pnt.setPen(QPen(Qt::white));
   QFont CurF("serif"); CurF.setPointSizeF(16), CurF.setBold(true), Pnt.setFont(CurF);
   int X = _Spacer, Y = _Spacer;
   QRect Rs = rect(); int Xs = _ArtWork->width() - 2*_Spacer;
// Write the text.
   QString Msg = AppName;
   QRect R = Pnt.boundingRect(Rs, 0, Msg);
   Pnt.drawText(X, Y, Xs, R.height(), Qt::AlignLeft | Qt::AlignTop, Msg), Y += R.height();
   CurF.setPointSizeF(10), CurF.setBold(false), Pnt.setFont(CurF);
   Msg = AppCopyRight, R = Pnt.boundingRect(Rs, 0, Msg);
   Pnt.drawText(X, Y, Xs, R.height(), Qt::AlignLeft | Qt::AlignTop, Msg), Y += R.height();
   Pnt.drawText(X, Y, Xs, R.height(), Qt::AlignLeft | Qt::AlignTop, tr("Version: ") + AppVersion), Y += R.height();
// Up from the bottom of the image.
   Y = _ArtWork->height() - R.height();
   Pnt.drawText(X, Y, Xs, R.height(), Qt::AlignLeft | Qt::AlignTop, tr("No Warranty. GNU General Public License.")), Y -= R.height();
   Pnt.drawText(X, Y, Xs, R.height(), Qt::AlignLeft | Qt::AlignTop, AppDomain);
}

// class About: public members
// ───────────────────────────
// Make a new About object.
About::About(QWidget *Sup): QDialog(Sup, Qt::Dialog | Qt::WindowCloseButtonHint) {
   setWindowTitle(tr("About..."));
// Create the display area.
   _ArtWork = new QPixmap(":/Artwork.png", 0);
// Create a button box.
   QDialogButtonBox *ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
   connect(ButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
// Set up the layout grid.
// Note: we add a spacer at to create an area in which to draw the image and zero the spacing between items.
// However, there is still a contentsMargin spacing around the grid itself.
// In order to leave a margin around the button bar, we size the draw area so as to include the margin.
// Because we will draw the image at the top left of client window, no margin will be shown around the image
// and the top spacer area will be used to create a spacer between the image and button bar instead.
   QGridLayout *Grid = new QGridLayout(this); Grid->setHorizontalSpacing(0), Grid->setVerticalSpacing(0);
   int X0, Y0, X1, Y1; Grid->getContentsMargins(&X0, &Y0, &X1, &Y1);
   QSpacerItem *DisplayArea = new QSpacerItem(_ArtWork->width() - X0 - X1, _ArtWork->height());
   Grid->addItem(DisplayArea, 0, 0), Grid->addWidget(ButtonBox, 1, 0);
// Hold the contents spacing.
   _Spacer = X0;
}

// Free the About object.
About::~About() {
   try {
      delete _ArtWork;
   } catch(...) { }
}
