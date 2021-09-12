// Asteroid Style Game: The driver program.
// Copyright (c) 2009 Andy Thomas, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QApplication>
#include "Arena.h"

int main(int AC, char **AV) {
   try {
      QApplication App(AC, AV);
      QPixmap PixMap(":/Artwork.bmp");
      Arena MainWin; MainWin.show();
      return App.exec();
   } catch(...) { return -1; }
}
