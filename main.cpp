#include <iostream>

#include "time.h"
#include "uct.h"


int main(int argc, char *argv[])
{
	libataxx::Position p("startpos");

	while(!p.gameover()) {
		uint64_t  start_ts = get_ms();

		uint64_t  n_played = 0;

		uct_node *root = new uct_node(nullptr, new libataxx::Position(p), libataxx::Move());

		for(;;) {
			uct_node *best = root->monte_carlo_tree_search();

			n_played++;

			if (get_ms() - start_ts > 10) {
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

				printf("played: %s\n", move.c_str());

				std::cout << p << std::endl;

				break;
			}
		}

		printf("# played: %lu\n", n_played);

		delete root;
	}

	return 0;
}
