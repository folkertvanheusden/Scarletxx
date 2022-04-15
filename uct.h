#include <map>

#include "libataxx/src/libataxx/position.hpp"


class uct_node
{
private:
	uct_node                 *const parent;
	std::map<libataxx::Move, uct_node *> children;
	const libataxx::Position *const position;

public:
	uct_node(uct_node *const parent, const libataxx::Position *const position);
	virtual ~uct_node();

	void add_child(const libataxx::Move & m);
};
