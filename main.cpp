#include "time.h"
#include "uct.h"

int main(int argc, char *argv[])
{
	uint64_t  start_ts = get_ms();

	uint64_t  n_played = 0;

	uct_node *root = new uct_node(nullptr, new libataxx::Position(), libataxx::Move());

	do {
		root->monte_carlo_tree_search();

		n_played++;
	}
	while(get_ms() - start_ts <= 1000);

	printf("%lu\n", n_played);

	delete root;

	return 0;
}
