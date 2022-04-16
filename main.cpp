#include <iostream>
#include <set>

#include "time.h"
#include "uct.h"


int main(int argc, char *argv[])
{
	libataxx::Position p("startpos");

	bool repetition = false;
	std::set<uint64_t> history;

	while(!p.gameover() && !repetition) {
		uint64_t  start_ts = get_ms();

		uint64_t  n_played = 0;

		uct_node *root = new uct_node(nullptr, new libataxx::Position(p), libataxx::Move());

		for(;;) {
			uct_node *best = root->monte_carlo_tree_search();

			n_played++;

			if (get_ms() - start_ts >= 2500) {
				std::string move;

				if (best) {
					p.makemove(best->get_causing_move());

					move = std::string(best->get_causing_move());
				}
				else {
					auto moves = p.legal_moves();

					auto cur_move = moves.at(random() % moves.size());

					move = std::string(cur_move);
				}

				uint64_t cur_hash = p.hash();

				repetition = history.find(cur_hash) != history.end();

				history.insert(cur_hash);

				printf("played: %s\n", move.c_str());

				// std::cout << p << std::endl;

				break;
			}
		}

		printf("# played: %f\n", n_played / 2.5);

		delete root;
	}

	return 0;
}
