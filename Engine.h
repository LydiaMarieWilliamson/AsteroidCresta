#ifndef OnceOnlyEngine_h
#define OnceOnlyEngine_h

// Asteroid Style Game: The engine for holding and updating the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <time.h>
#include <vector>
#include "Objects.h"

namespace asteroid {
// Game Presets
// Used to calculate size of Kuypier region.
const int KUYP_DIV = 6;
// Timings in seconds: pause before new life, default text label life, pause before gameOver().
const int NEW_LIFE_PAUSE = 2, DEF_TEXT_SECS = 2, END_GAME_PAUSE = 3;
// Maxima: fireCharge(), object speed (controls the game speed), alien speed.
const int CHARGE_MAX = 6, MAX_SPEED = 14, MAX_ALIEN_SPEED = 8;
// Timings in ticks: to half max rock speed, before rocks can explode, before fire charge increases.
const int HALF_MAX_TICK = 1500, ROCK_LIFE_LIMIT = 25, CHARGE_MOD = 12;
// Ship rotate delta per tick.
const double SHIP_ROTATE_DELTA = 9.0;
// Probabiities: rock and alien creation at 0.5 HALF_MAX_TICK; spontaneous rock explosion.
const double ROCK_PROB = 0.02, ALIEN_PROB = 0.005, ROCK_EXPLODE_PROB = 0.001;
// Game speed controls: ship thrust factor, ship file recoil, alien thrust factor, initial rock speed factor.
const double SHIP_THRUST_MULT = 0.25, FIRE_RECOIL_MULT = 0.01, ALIEN_THRUST_MULT = 1.0, ROCK_SPEED_MULT = 0.735;

// The game logic engine and game object roster
// ────────────────────────────────────────────
class Engine {
private:
   std::vector<asteroid::Obj *> m_objects;
   int m_tickCnt, m_shipIdx, m_lives, m_initRocks;
   int m_score, m_lastScore, m_highScore;
   int m_width, m_height;
   time_t m_newLifeWait, m_demoEndMark, m_gameOverMark;
   bool m_active, m_diedSnd, m_alienSnd;
   OType m_explosionSnd;
   double m_diff;
#if 0
   void m_removeDead(); //(@) Not used anywhere.
#endif
   void m_empty(bool killNow);
   bool m_collision(const Obj &o1, const Obj &o2) const;
   void m_rebound(const Obj &o1, const Obj &o2, ObjPos &nd1, ObjPos &nd2) const;
   void m_stateTick();
   int m_typeCnt(OType t) const;
   Ship *m_getShip() const;
public:
   Engine();
   virtual ~Engine();
// Add type-ot objects to the game.
// For internal use only.
   Obj *add(OType ot);
   Obj *add(OType ot, const ObjPos &pos, const ObjPos &dir = ObjPos());
   Obj *addKuypier(OType ot, int tick);
// Access internal objects.
// These are needed in order to render objects onto the screen device.
   size_t objCnt() const;
   Obj *objAtIdx(size_t n) const;
// State control.
   bool active() const;
   bool demo() const;
   bool playing() const;
   bool gameOver() const;
   void startGame(int rocks = 10);
   void startDemo(time_t secs = 20, int rocks = 10);
   void stop();
   void tick();
   int lives() const;
   int score() const;
   int lastScore() const;
   int hiscore() const;
   void hiscore(int hs);
   double difficulty() const;
   void difficulty(const double &dif);
// The game and ship control input.
   void addAlienCheat();
   void rotate(int r);
   void thrust(bool on);
   void fire();
   void reload();
   int charge() const;
// The sound flags (poll after calling tick()).
   OType rockExplodeSnd() const;
   bool fireSnd() const;
   bool thrustSnd() const;
   bool alienSnd() const;
   bool diedSnd() const;
// The game playing area methods.
   void getPlayDims(int *w, int *h) const;
   void setPlayDims(int w, int h);
   int minDim() const;
};
} // end of namespace Asteroid

#endif // OnceOnly
