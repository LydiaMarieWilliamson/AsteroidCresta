#ifndef OnceOnlyAbout_h
#define OnceOnlyAbout_h

// Asteroid Style Game: The about window.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QDialog>

class QPixmap;

class About: public QDialog {
Q_OBJECT
private:
   QPixmap *_ArtWork;
   int _Spacer;
protected:
   virtual void paintEvent(QPaintEvent *Ev);
public:
   About(QWidget *Sup);
   ~About();
};

#endif // OnceOnly
