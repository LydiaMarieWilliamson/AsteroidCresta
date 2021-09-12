// Asteroid Style Game: The driver program.
// Copyright (c) 2009 Andy Thomas, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QApplication>
#include "Arena.h"

int main(int argc, char **argv) {
   try {
      QApplication app(argc, argv);
      QPixmap pixmap(":/Artwork.bmp");

      MainWindow main_win;
      main_win.show();

      return app.exec();
   } catch(...) { return -1; }
}
