#include <map>

#include "libataxx/src/libataxx/position.hpp"


class uct_node
{
private:
	uct_node                 *const parent    { nullptr };
	std::map<libataxx::Move, uct_node *> children;
	const libataxx::Position *const position  { nullptr };
	std::vector<libataxx::Move>    *unvisited { nullptr };
	uint64_t                        visited   { 0 };
	uint64_t                        score     { 0 };

	uct_node *add_child(const libataxx::Move & m);

public:
	uct_node(uct_node *const parent, const libataxx::Position *const position);
	virtual ~uct_node();

	uint64_t  get_visit_count();
	double    get_score();

	uct_node *pick_unvisited();
	uct_node *traverse(uct_node *node);
	uct_node *best_uct();

	bool      fully_expanded();
};
