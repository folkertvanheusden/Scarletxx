#include <map>

#include "libataxx/src/libataxx/position.hpp"


class uct_node
{
private:
	uct_node                 *const parent   { nullptr };
	std::map<libataxx::Move, uct_node *> children;
	const libataxx::Position *const position { nullptr };
	uint64_t                        visited  { 0 };
	uint64_t                        score    { 0 };

public:
	uct_node(uct_node *const parent, const libataxx::Position *const position);
	virtual ~uct_node();

	void      add_child(const libataxx::Move & m);

	uint64_t  get_visit_count();
	uint64_t  get_score();

	uct_node *pick_unvisited();
};
