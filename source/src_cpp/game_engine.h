//---------------------------------------------------------------------------
// PROJECT     : Asteroid Style Game
// FILE NAME   : game_engine.h
// DESCRIPTION : Engine holds & updates game artifacts
// COPYRIGHT   : Big Angry Dog (C) 2009
// This file is part of the "Asteroid Cresta" program.
// Asteroid Cresta is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Asteroid Cresta is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with Asteroid Cresta.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// HEADER GUARD
//---------------------------------------------------------------------------
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

//---------------------------------------------------------------------------
// INCLUDES
//---------------------------------------------------------------------------
#include <time.h>
#include <vector>
#include "game_objects.h"

namespace asteroid {

//---------------------------------------------------------------------------
// NON-CLASS MEMBERS
//---------------------------------------------------------------------------

// GAME CONSTANTS
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

//---------------------------------------------------------------------------
// CLASS Engine (holds a list of game objects & implements logic)
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
}
#endif
