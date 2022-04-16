// written by folkert van heusden <mail@vanheusden.com>
// this code is public domain
#include <map>

#include "libataxx/src/libataxx/position.hpp"


class uct_node
{
private:
	uct_node                 *const parent    { nullptr };
	const libataxx::Position *const position  { nullptr };
	const libataxx::Move            causing_move;

	std::vector<std::pair<libataxx::Move, uct_node *> > children;
	std::vector<libataxx::Move>    *unvisited { nullptr };
	uint64_t                        visited   { 0 };
	uint64_t                        score     { 0 };

	uct_node *add_child(const libataxx::Move & m);
	void      update_stats(const int result);

public:
	uct_node(uct_node *const parent, const libataxx::Position *const position, const libataxx::Move & causing_move);
	virtual ~uct_node();

	uint64_t  get_visit_count();
	double    get_score();

	uct_node *get_parent();
	uct_node *pick_unvisited();
	uct_node *pick_for_revisit();
	uct_node *traverse();
	uct_node *best_uct();
	uct_node *best_child();
	void      backpropagate(uct_node *const node, const int result);
	bool      fully_expanded();
	uct_node *monte_carlo_tree_search();
	libataxx::Position playout(const uct_node *const leaf);

	const libataxx::Position *get_position() const;

	const libataxx::Move get_causing_move() const;
};
