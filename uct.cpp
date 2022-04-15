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

	uct_node *new_node = add_child(unvisited->back());

	unvisited->pop_back();

	if (unvisited->empty()) {
		delete unvisited;

		unvisited = nullptr;
	}

	return new_node;
}

bool uct_node::fully_expanded()
{
	return unvisited == nullptr;
}

// TODO
uct_node *uct_node::best_uct()
{
	uct_node *best       = nullptr;
	double    best_score = 0;

	for(auto u : children) {
		uint64_t current_score = u.second->get_score();

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
	uct_node *last_best = nullptr;

	while(node != nullptr && node->fully_expanded()) {
		last_best = node;

		node = node->best_uct();
	}

	if (node == nullptr)  // terminal node?
		return last_best;

	return node->pick_unvisited();
}

uct_node *uct_node::best_child()
{
	uct_node *best       = nullptr;
	uint64_t  best_count = 0;

	for(auto u : children) {
		uint64_t count = u.second->get_visit_count();

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

	int simulation_result = get_position()->score() > 0;

	backpropagate(leaf, simulation_result);
		
	return best_child();
}

const libataxx::Move uct_node::get_causing_move() const
{
	return causing_move;
}
