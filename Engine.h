#ifndef OnceOnlyEngine_h
#define OnceOnlyEngine_h

// Asteroid Style Game: The engine for holding and updating the game artifacts.
// Copyright (c) 2009 Big Angry Dog, (c) 2016-2018 Darth Ninja, (c) 2021 Darth Spectra
#include <time.h>
#include <vector>
#include "Objects.h"

namespace asteroid {
// Game Presets
const int KUYP_DIV = 6;  // Used to calculate size of Kuypier region
const int NEW_LIFE_PAUSE = 2; // Pause (secs) before new life
const int DEF_TEXT_SECS = 2; // Default text label life (secs)
const int END_GAME_PAUSE = 3; // Pause (secs) before gameOver() is true
const int HALF_MAX_TICK = 1500; // Ticks to half max rock speed
const int CHARGE_MAX = 6; // Max value of fireCharge()
const int MAX_SPEED = 14; // Max speed of any object (controls game speed)
const int MAX_ALIEN_SPEED = 8; // Max speed of alien
const int ROCK_LIFE_LIMIT = 25; // Number of ticks before rocks can explode
const int CHARGE_MOD = 12; // Ticks before fire charge increases
const double SHIP_ROTATE_DELTA = 9.0; // Ship rotate delta per tick
const double ROCK_PROB = 0.02; // Rock creation prob at 0.5 HALF_MAX_TICK
const double ALIEN_PROB = 0.005; // Alien creation prob at 0.5 HALF_MAX_TICK
const double ROCK_EXPLODE_PROB = 0.001; // Probability a rock will explode on its own
const double SHIP_THRUST_MULT = 0.25; // Ship thrust factor (controls game speed)
const double FIRE_RECOIL_MULT = 0.01; // Ship file recoil (controls game speed)
const double ALIEN_THRUST_MULT = 1.0; // Alien thrust factor (controls game speed)
const double ROCK_SPEED_MULT = 0.735; // Initial rock speed factor (controls game speed)

// The game logic engine and game object roster
class Engine
{
private:
  std::vector<asteroid::Obj*> m_objects;
  int m_tickCnt, m_shipIdx, m_lives, m_initRocks;
  int m_score, m_lastScore, m_highScore;
  int m_width, m_height;
  time_t m_newLifeWait, m_demoEndMark, m_gameOverMark;
  bool m_active, m_diedSnd, m_alienSnd;
  OType m_explosionSnd;
  double m_diff;

  void m_removeDead();
  void m_empty(bool killNow);
  bool m_collision(const Obj& o1, const Obj& o2) const;
  void m_rebound(const Obj& o1, const Obj& o2,
    ObjPos& nd1, ObjPos& nd2) const;
  void m_stateTick();
  int m_typeCnt(OType t) const;
  Ship* m_getShip() const;
public:
  Engine();
  virtual ~Engine();

  // Add objects to game.
  // For internal use only.
  Obj* add(OType ot);
  Obj* add(OType ot, const ObjPos& pos,
    const ObjPos& dir = ObjPos());
  Obj* addKuypier(OType ot, int tick);

  // Access internal objects.
  // These are needed in order to render
  // objects onto screen device
  size_t objCnt() const;
  Obj* objAtIdx(size_t n) const;

  // State control
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
  void difficulty(const double& dif);

  // Game & ship control input
  void addAlienCheat();
  void rotate(int r);
  void thrust(bool on);
  void fire();
  void reload();
  int charge() const;

  // Sound flags (poll after calling tick)
  OType rockExplodeSnd() const;
  bool fireSnd() const;
  bool thrustSnd() const;
  bool alienSnd() const;
  bool diedSnd() const;

  // Game playing area methods
  void getPlayDims(int* w, int* h) const;
  void setPlayDims(int w, int h);
  int minDim() const;
};
} // end of namespace Asteroid

#endif // OnceOnly
