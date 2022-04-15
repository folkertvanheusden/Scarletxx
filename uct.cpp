#include "uct.h"

uct_node::uct_node(uct_node *const parent, const libataxx::Position *const position) :
	parent(parent),
	position(position)
{
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
