#ifndef OnceOnlyVersion_h
#define OnceOnlyVersion_h

// Asteroid Style Game: The application version information.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <QString>

const QString AppName = "Asteroid Cresta";
const QString AppInternalName = "Asteroid Style Game";
#if 1
const QString AppVersion = "1.9.2";
const QString AppCopyRight = "Copyright \xA9 2021 Darth Spectra et al.";
const QString AppAuthor = "Andy Thomas, Darth Ninja, Darth Spectra";
const QString AppCompany = "Asteroid Cresta II";
const QString AppDomain = "github.com (Provisional)";
const QString AppUrl = "http://github.com/LydiaMarieWilliamson/AsteroidCresta";
#else
const QString AppVersion = "1.8.0";
const QString AppCopyRight = "Copyright \xA9 2011 Big Angry Dog";
const QString AppAuthor = "Andy Thomas";
const QString AppCompany = "Big Angry Dog";
const QString AppDomain = "BigAngryDog.com";
const QString AppUrl = "http://www.BigAngryDog.com";
#endif

#endif // OnceOnly
