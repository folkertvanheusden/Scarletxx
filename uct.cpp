#include "uct.h"

uct_node::uct_node(uct_node *const parent, const libataxx::Position *const position) :
	parent(parent),
	position(position),
	unvisited(new std::vector<libataxx::Move>(position->legal_moves()))
{
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

	uct_node *new_node = new uct_node(this, new_position);

	children.insert({ m, new_node });

	return new_node;
}

uint64_t uct_node::get_visit_count()
{
	return visited;
}

uint64_t uct_node::get_score()
{
	return score;
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
