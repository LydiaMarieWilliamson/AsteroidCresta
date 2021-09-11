#ifndef OnceOnlyVersion_h
#define OnceOnlyVersion_h

// Asteroid Style Game: The application version information.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QString>

const QString APP_NAME_STR = "Asteroid Cresta";
const QString APP_INTERNAL_NAME_STR = "Asteroid Style Game";
#if 1
const QString APP_VERSION_STR = "1.9.0";
const QString APP_COPYRIGHT_STR = "Copyright \xA9 2011 Big Angry Dog, \xA9 2016 Darth Ninja, \xA9 2021 Darth Spectra";
const QString APP_AUTHOR_STR = "Andy Thomas (original)";
#else
const QString APP_VERSION_STR = "1.8.0";
const QString APP_COPYRIGHT_STR = "Copyright \xA9 2011 Big Angry Dog";
const QString APP_AUTHOR_STR = "Andy Thomas";
#endif
const QString APP_COMPANY_STR = "Big Angry Dog";
const QString APP_DOMAIN_STR = "BigAngryDog.com";
const QString APP_URL_STR = "http://www.BigAngryDog.com";

#endif // OnceOnly
