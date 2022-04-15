#include <cfloat>

#include "uct.h"


uct_node::uct_node(uct_node *const parent, const libataxx::Position *const position, const libataxx::Move & causing_move) :
	parent(parent),
	position(position),
	causing_move(causing_move),
	unvisited(new std::vector<libataxx::Move>(position->legal_moves()))
{
	if (unvisited->empty()) {
		delete unvisited;

		unvisited = nullptr;
	}
}

uct_node::~uct_node()
{
	delete position;

	for(auto u : children)
		delete u.second;
}

uct_node *uct_node::add_child(const libataxx::Move & m)
{
	libataxx::Position *new_position = new libataxx::Position(
			position->black(),
			position->white(),
			position->gaps(),
			position->halfmoves(),
			position->fullmoves(),
			position->turn());

	new_position->makemove(m);

	uct_node *new_node = new uct_node(this, new_position, m);

	children.insert({ m, new_node });

	return new_node;
}

uint64_t uct_node::get_visit_count()
{
	return visited;
}

double uct_node::get_score()
{
	if (visited == 0)
		return -1;

	return double(score) / visited;
}

uct_node *uct_node::pick_unvisited()
{
	if (unvisited == nullptr)
		return nullptr;

	printf("# unvisited: %zu\n", unvisited->size());

	uct_node *new_node = add_child(unvisited->back());

	unvisited->pop_back();

	if (unvisited->empty()) {
		delete unvisited;

		unvisited = nullptr;
	}
	else {
		printf("# unvisited: %zu, new node %p\n", unvisited->size(), new_node);
	}

	return new_node;
}

uct_node *uct_node::pick_for_revisit()
{
}

bool uct_node::fully_expanded()
{
	return unvisited == nullptr;
}

// TODO
uct_node *uct_node::best_uct()
{
	uct_node *best       = nullptr;
	double    best_score = -DBL_MAX;

	for(auto u : children) {
		double current_score = u.second->get_score();
		printf("score: %f\n", current_score);

		if (current_score > best_score) {
			best_score = current_score;
			best = u.second;
		}
	}

	return best;
}

uct_node *uct_node::traverse()
{
	uct_node *node      = this;

	printf("traverse FE: %d\n", node->fully_expanded());

	while(node->fully_expanded()) {
		uct_node *next = node->best_uct();

		if (!next)
			break;

		node = next;

		printf("traverse it best_uct: %p\n", node);
	}

	printf("traverse na tree lopen: %p\n", node);

	uct_node *chosen    = nullptr;

	if (node) {
		chosen = node->pick_unvisited();

		printf("traverse node: chosen %p\n", chosen);

		if (!chosen)
			chosen = node->pick_for_revisit();
	}

	return chosen;
}

uct_node *uct_node::best_child()
{
	uct_node *best       = nullptr;
	int64_t  best_count = -1;

	for(auto u : children) {
		int64_t count = u.second->get_visit_count();

		if (count > best_count) {
			best_count = count;
			best = u.second;
		}
	}

	return best;
}

void uct_node::update_stats(const int result)
{
	visited++;

	score += result;
}

// can be made non-recursive
void uct_node::backpropagate(uct_node *const node, const int result)
{
	if (!node)
		return;

	update_stats(result);

	backpropagate(parent, result);
}

const libataxx::Position *uct_node::get_position() const
{
	return position;
}

uct_node *uct_node::monte_carlo_tree_search()
{
	uct_node *leaf = traverse();

	printf("monte_carlo_tree_search leaf: %p\n", leaf);

	int simulation_result = leaf->get_position()->score() > 0;

	printf("monte_carlo_tree_search sim result: %d\n", simulation_result);

	backpropagate(leaf, simulation_result);

	uct_node *result = best_child();

	printf("monte_carlo_tree_search best_child: %p\n", result);

	return result;
}

const libataxx::Move uct_node::get_causing_move() const
{
	return causing_move;
}
