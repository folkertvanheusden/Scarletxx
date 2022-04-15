#include "uct.h"

uct_node::uct_node(uct_node *const parent, const libataxx::Position *const position) :
	parent(parent),
	position(position)
{
	// TODO: setup 'children'; pre-fill with each move
}

uct_node::~uct_node()
{
	delete position;

	for(auto u : children)
		delete u.second;
}

void uct_node::add_child(const libataxx::Move & m)
{
	libataxx::Position *new_position = new libataxx::Position(
			position->black(),
			position->white(),
			position->gaps(),
			position->halfmoves(),
			position->fullmoves(),
			position->turn());

	new_position->makemove(m);

	children.insert({ m, new uct_node(this, new_position) });
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
	// check for each child if it had played already
	for(auto & child : children) {
		if (child.second->get_visit_count() == 0)
			return child.second;
	}

	return nullptr;
}
