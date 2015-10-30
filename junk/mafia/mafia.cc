#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cassert>

// With this file with can compute the winning odds of the mafia in the
// mafia game. See the paper:
// A mathematical model of the Mafia game: http://arxiv.org/abs/1009.1031

// This simulation approximates the results of the paper surprisingly well.

// Parameters.

// How maby players in the game?
const int PARAM_N_PLAYERS = 7;
const int PARAM_N_MAFIA = 2;

using namespace std;

enum PlayerType { CITIZEN, MAFIA };

class MafiaGame {
  enum TurnsType { DAY = 0, NIGHT };

 public:
  MafiaGame(int n_players, int n_mafia)
      : param_mafia_(n_mafia), param_citizen_(n_players - n_mafia) {
    assert(param_mafia_ > 0);
    assert(param_citizen_ > 0);
  }

  // Simulate a round. Returns the type of the winner (MAFIA or CITIZEN).
  PlayerType Play() {
    // Starting conditions of the game.
    int mafia = param_mafia_;
    int citizen = param_citizen_;
    // A vector with all players.
    PlayerType player[mafia + citizen];
    // A vector that holds the player votes.
    int votes[mafia + citizen];
    // Keep track of ties.
    int tied_votes[mafia + citizen];
    int turn = DAY;
    while (1) {
      // Do we have winners?
      if (mafia > citizen) {
        return MAFIA;
      }
      if (0 == mafia) {
        return CITIZEN;
      }
      if (turn == NIGHT) {
        citizen--;
        turn = DAY;
        continue;
      }
      // Fill player state
      for (int i = 0; i < mafia; ++i) {
        player[i] = MAFIA;
        votes[i] = 0;
      }
      for (int i = mafia; i < mafia + citizen; ++i) {
        player[i] = CITIZEN;
        votes[i] = 0;
      }
      // Let's start with a simulaion for simplicity. We should use better math.
      // No winners yet. Let's the voting begin.
      for (int who = 0; who < mafia + citizen; ++who) {
        while (1) {
          int chosen = random_float() * (mafia + citizen);
          if (chosen == who) {
            // People don't vote for themselves
            continue;
          }
          // Note that I allow mafia players to vote against other
          // mafia players. If I don't do this, I din't match paper
          // results. This is counter intuitive but I think it makes
          // sense.
          votes[chosen]++;
          break;
        }
      }
      // Find the maximum value, the most voted player.
      const int* maximum = max_element(votes, votes + mafia + citizen);
      // Solve ties.
      int n_tied = 0;
      for (int i = 0; i < mafia + citizen; ++i) {
        if (*maximum == votes[i]) {
          tied_votes[n_tied++] = i;
        }
      }
      // Get the lynched player.
      if (player[tied_votes[static_cast<int>(random_float() * n_tied)]] ==
          MAFIA) {
        --mafia;
      } else {
        --citizen;
      }
      turn = NIGHT;
    }
  }

 private:
  // I know this RNG is not great.
  double random_float() { return double(random()) / (double(RAND_MAX) + 1.0); }
  // How many mafia players.
  int param_mafia_;
  // How many citizens.
  int param_citizen_;
};

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  int mafia_wins = 0;
  int citizen_wins = 0;
  MafiaGame game(PARAM_N_PLAYERS, PARAM_N_MAFIA);
  int turn = 1;
  cout << "This will run forever, with time better approximations are reached." << endl;
  while (1) {
    if (game.Play() == MAFIA) {
      ++mafia_wins;
    } else {
      ++citizen_wins;
    }
    if (turn % 1000000 == 0) {
      cout << "Will the mafia win? Probability: "
           << (float(mafia_wins) / float(citizen_wins + mafia_wins)) << endl;
    }
    turn++;
  }
}
