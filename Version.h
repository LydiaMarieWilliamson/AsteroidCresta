#ifndef OnceOnlyVersion_h
#define OnceOnlyVersion_h

// Asteroid Style Game: The application version information.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QString>

const QString AppName = "Asteroid Cresta";
const QString AppInternalName = "Asteroid Style Game";
#if 1
const QString AppVersion = "1.9.0";
const QString AppCopyRight = "Copyright \xA9 2011 Big Angry Dog, \xA9 2016 Darth Ninja, \xA9 2021 Darth Spectra";
const QString AppAuthor = "Andy Thomas (original)";
#else
const QString AppVersion = "1.8.0";
const QString AppCopyRight = "Copyright \xA9 2011 Big Angry Dog";
const QString AppAuthor = "Andy Thomas";
#endif
const QString AppCompany = "Big Angry Dog";
const QString AppDomain = "BigAngryDog.com";
const QString AppUrl = "http://www.BigAngryDog.com";

#endif // OnceOnly
