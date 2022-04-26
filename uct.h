// written by folkert van heusden <mail@vanheusden.com>
// this code is public domain
#include <map>
#include <optional>
#include <shared_mutex>

#include "libataxx/src/libataxx/position.hpp"


class uct_node
{
private:
	uct_node                 *const parent    { nullptr };
	libataxx::Position              position;
	const std::optional<libataxx::Move> causing_move;

	std::vector<std::pair<libataxx::Move, uct_node *> > children;
	std::vector<libataxx::Move>     unvisited;
	uint64_t                        visited   { 0 };
	double                          score     { 0. };

	uct_node *add_child(const libataxx::Move & m);

	uct_node *get_parent();
	uct_node *pick_unvisited();
	uct_node *traverse();
	uct_node *best_uct();
	void      backpropagate(uct_node *const node, double result);
	bool      fully_expanded();
	double    get_score();
	libataxx::Position playout(const uct_node *const leaf);

public:
	uct_node(uct_node *const parent, const libataxx::Position & position, const std::optional<libataxx::Move> & causing_move);
	virtual ~uct_node();

	void      monte_carlo_tree_search();

	const libataxx::Position get_position() const;

	uct_node *best_child() const;

	const libataxx::Move get_causing_move() const;

	const std::vector<std::pair<libataxx::Move, uct_node *> > & get_children() const;
	void      update_stats(const uint64_t visited, const double score);
	uint64_t  get_visit_count();
	double    get_score_count();
};
