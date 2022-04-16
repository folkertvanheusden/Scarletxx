// written by folkert van heusden <mail@vanheusden.com>
// this code is public domain
#include <iostream>
#include <set>
#include <string>
#include <string.h>
#include <thread>
#include <vector>

#include "time.h"
#include "uct.h"


std::vector<std::string> split(const std::string & in_in, const std::string & splitter)
{
	std::string in = in_in;

	std::vector<std::string> out;
	size_t splitter_size = splitter.size();

	for(;;)
	{
		size_t pos = in.find(splitter);
		if (pos == std::string::npos)
			break;

		std::string before = in.substr(0, pos);
		out.push_back(before);

		size_t bytes_left = in.size() - (pos + splitter_size);
		if (bytes_left == 0)
		{
			out.push_back("");
			return out;
		}

		in = in.substr(pos + splitter_size);
	}

	if (in.size() > 0)
		out.push_back(in);

	return out;
}

libataxx::Move calculate_move_helper(const uint64_t start_ts, const libataxx::Position & p, uct_node *const root, const unsigned think_time)
{
	for(;;) {
		uct_node *best = root->monte_carlo_tree_search();

		if (get_ms() - start_ts >= think_time) {

			libataxx::Move move;

			if (best)
				move = best->get_causing_move();
			else {
				auto moves = p.legal_moves();

				move = moves.at(random() % moves.size());
			}

			return move;
		}
	}
}

libataxx::Move calculate_move(const libataxx::Position & p, const unsigned think_time)
{
	uint64_t  start_ts = get_ms();

	uct_node *root = new uct_node(nullptr, new libataxx::Position(p), libataxx::Move());

	std::thread *threads[5] { nullptr };

	unsigned thread_think_time = think_time > 5 ? think_time - 5 : 1;

	for(int i=0; i<5; i++)
		threads[i] = new std::thread([start_ts, p, root, thread_think_time]{ calculate_move_helper(start_ts, p, root, thread_think_time); });

	auto move = calculate_move_helper(start_ts, p, root, think_time);

	for(int i=0; i<5; i++) {
		threads[i]->join();

		delete threads[i];
	}

	delete root;

	return move;
}

int main(int argc, char **argv)
{
	auto pos = libataxx::Position("startpos");

	for(;;) {
		char buffer[65536];
		if (!fgets(buffer, sizeof buffer, stdin))
			break;

		char *lf = strchr(buffer, '\n');
		if (lf)
			*lf = 0x00;

		std::vector<std::string> parts = split(buffer, " ");

		if (parts.size() == 0)
			continue;

		if (parts.at(0) == "uai") {
			printf("id name Scarletxx %s\n", "0.004alpha");
			printf("id author Folkert van Heusden\n");
			printf("uaiok\n");
		}
		else if (parts.at(0) == "uainewgame") {
			pos = libataxx::Position("startpos");
		}
		else if (parts.at(0) == "position") {
			bool moves = false;

			for(size_t i=1; i<parts.size();) {
				if (parts.at(i) == "fen") {
					std::string fen;

					for(size_t f = i + 1; f<parts.size(); f++)
						fen += parts.at(f) + " ";

					pos = libataxx::Position(fen);

					break;
				}
				else if (parts.at(i) == "startpos") {
					pos = libataxx::Position("startpos");
					i++;
				}
				else if (parts.at(i) == "moves") {
					moves = true;
					i++;
				}
				else if (moves) {
					while(i < parts.size() && parts.at(i).size() < 4)
						i++;

					libataxx::Move new_move = libataxx::Move::from_uai(parts.at(i));

					libataxx::Move moves[libataxx::max_moves];
					const int num_moves = pos.legal_moves(moves);
					for (int k = 0; k < num_moves; ++k) {
						if (moves[k] == new_move) {
							pos.makemove(new_move);
							break;
						}
					}

					i++;
				}
				else {
				}
			}
		}
		else if (parts.at(0) == "go") {
			int moves_to_go = 40 - pos.halfmoves() / 2;
			int w_time = 0, b_time = 0, w_inc = 0, b_inc = 0;
			bool time_set = false;

			for(size_t i=1; i<parts.size(); i++) {
				if (parts.at(i) == "movetime") {
					w_time = b_time = atoi(parts.at(++i).c_str());
					time_set = true;
				}
				else if (parts.at(i) == "wtime")
					w_time = atoi(parts.at(++i).c_str());
				else if (parts.at(i) == "btime")
					b_time = atoi(parts.at(++i).c_str());
				else if (parts.at(i) == "winc")
					w_inc = atoi(parts.at(++i).c_str());
				else if (parts.at(i) == "binc")
					b_inc = atoi(parts.at(++i).c_str());
				else if (parts.at(i) == "movestogo")
					moves_to_go = atoi(parts.at(++i).c_str());
			}

			int think_time = 0;
			if (time_set)
				think_time = (pos.turn() == libataxx::Side::White ? w_time : b_time) * 0.95;
			else {
				int cur_n_moves = moves_to_go <= 0 ? 40 : moves_to_go;

				int time_inc = pos.turn() == libataxx::Side::White ? w_inc : b_inc;

				int ms = pos.turn() == libataxx::Side::White ? w_time : b_time;
				think_time = (ms + (cur_n_moves - 1) * time_inc) / double(cur_n_moves + 7);

				int limit_duration_min = ms / 15;
				if (think_time > limit_duration_min)
					think_time = limit_duration_min;
			}

			// 50ms overhead
			if (think_time > 50)
				think_time -= 50;

			libataxx::Move move = calculate_move(pos, think_time);

			std::cout << "bestmove " << move << std::endl;
		}
		else if (parts.at(0) == "isready")
			std::cout << "readyok" << std::endl;
		else if (parts.at(0) == "quit") {
			break;
		}
		else {
			std::cout << "Invalid command: " << buffer << std::endl;
		}

		fflush(NULL);
	}

	return 0;
}
