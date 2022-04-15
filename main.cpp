#include "time.h"
#include "uct.h"

int main(int argc, char *argv[])
{
	uint64_t  start_ts = get_ms();

	uint64_t  n_played = 0;

	uct_node *root = new uct_node(nullptr, new libataxx::Position("startpos"), libataxx::Move());

	do {
		uct_node *best = root->monte_carlo_tree_search();

		if (best) {
			std::string move = std::string(best->get_causing_move());

			printf("%s %lu %f\n", move.c_str(), best->get_visit_count(), best->get_score());
		}

		n_played++;
	}
	while(get_ms() - start_ts <= 1000);

	printf("# played: %lu\n", n_played);

	delete root;

	return 0;
}
