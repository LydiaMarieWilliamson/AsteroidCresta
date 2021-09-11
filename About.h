#ifndef OnceOnlyAbout_h
#define OnceOnlyAbout_h

// Asteroid Style Game: The about window.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QDialog>

class QPixmap;

class AboutWindow: public QDialog {
Q_OBJECT
private:
   QPixmap *mp_artwork;
   int m_spacer;
protected:
   virtual void paintEvent(QPaintEvent *event);
public:
   AboutWindow(QWidget *parent);
   ~AboutWindow();
};

#endif // OnceOnly
