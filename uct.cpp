// written by folkert van heusden <mail@vanheusden.com>
// this code is public domain
#include <algorithm>
#include <cfloat>
#include <random>
#include <mutex>

#include "uct.h"


auto produce_seed()
{
	std::vector<unsigned int> random_data(std::mt19937::state_size);

	std::random_device source;
	std::generate(std::begin(random_data), std::end(random_data), [&](){return source();});

	return std::seed_seq(std::begin(random_data), std::end(random_data));
}

thread_local auto mt_seed = produce_seed();
thread_local std::mt19937_64 gen { mt_seed };

uct_node::uct_node(uct_node *const parent, const libataxx::Position *const position, const libataxx::Move & causing_move) :
	parent(parent),
	position(position),
	causing_move(causing_move),
	unvisited(new std::vector<libataxx::Move>(position->legal_moves()))
{
}

uct_node::~uct_node()
{
	delete position;

	for(auto u : children)
		delete u.second;

	delete unvisited;
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

	children.push_back({ m, new_node });

	return new_node;
}

uint64_t uct_node::get_visit_count()
{
	return visited;
}

double uct_node::get_score_count()
{
	return score;
}

void uct_node::update_stats(const uint64_t visited, const double score)
{
	this->visited += visited;
	this->score   += score;
}

double uct_node::get_score()
{
	assert(visited);

	double UCTj = score / visited;

	constexpr double sqrt_2 = sqrt(2.0);

	UCTj += sqrt_2 * sqrt(log(parent->get_visit_count()) / visited);

	return UCTj;
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

uct_node *uct_node::best_uct()
{
	uct_node *best       = nullptr;
	double    best_score = -DBL_MAX;

	for(auto u : children) {
		double current_score = u.second->get_score();

		if (current_score > best_score) {
			best_score = current_score;
			best = u.second;
		}
	}

	// TODO: - move best_uct node to the front of the children-vector?
	//       - make children a map sorted by score?

	return best;
}

uct_node *uct_node::traverse()
{
	uct_node *node   = this;

	while(node->fully_expanded()) {
		uct_node *next = node->best_uct();

		if (!next)
			break;

		node = next;
	}

	uct_node *chosen = nullptr;

	if (node && node->get_position()->gameover() == false)
		chosen = node->pick_unvisited();

	return chosen;
}

uct_node *uct_node::best_child() const
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

uct_node *uct_node::get_parent()
{
	return parent;
}

void uct_node::backpropagate(uct_node *const leaf, double result)
{
	uct_node *node = leaf;

	do {
		node->update_stats(1, result);

		result = 1. - result;

		node = node->get_parent();
	}
	while(node);
}

const libataxx::Position *uct_node::get_position() const
{
	return position;
}

libataxx::Position uct_node::playout(const uct_node *const leaf)
{
	libataxx::Position position = *leaf->get_position();

	while(!position.gameover()) {
		auto moves = position.legal_moves();

		std::uniform_int_distribution<> rng(0, moves.size() - 1);

		position.makemove(moves.at(rng(gen)));
	}

	return position;
}

uct_node *uct_node::monte_carlo_tree_search()
{
	uct_node *leaf = traverse();
	if (!leaf)
		return nullptr;

	auto playout_terminal_position = playout(leaf);

	libataxx::Side side = playout_terminal_position.turn();

	libataxx::Result result = playout_terminal_position.result();

	assert(result != libataxx::Result::None);

	double simulation_result = 0.;

	if ((result == libataxx::Result::BlackWin && side == libataxx::Side::Black) || (result == libataxx::Result::WhiteWin && side == libataxx::Side::White))
		simulation_result = 1.0;
	else if (result == libataxx::Result::Draw)
		simulation_result = 0.5;

	backpropagate(leaf, simulation_result);

	return best_child();
}

const libataxx::Move uct_node::get_causing_move() const
{
	return causing_move;
}

const std::vector<std::pair<libataxx::Move, uct_node *> > & uct_node::get_children() const
{
	return children;
}
