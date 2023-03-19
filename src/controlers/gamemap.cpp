#include "gamemap.hpp"

#include <cstdlib>
#include <iostream>
#include <utility>
#include <algorithm>

namespace controler{
	bool entropy_compare(std::pair<int,int> i1, std::pair<int,int> i2)
	{
		return (i1.second < i2.second);
	}

	WaveFuncMap::WaveFuncMap(int _size, int number_end_points): size(_size), number_end_points(number_end_points){
		wave_map.reserve(size*size);
	}
	WaveFuncMap::~WaveFuncMap(){}

	auto WaveFuncMap::generate() -> std::vector<char> {
		//std::cout << "initilization" << std::endl;
		reset();
		//std::cout << "placing end points" << std::endl;
		for(int i = 0; i < number_end_points; i++){
			place_end_point();
		}
		//std::cout << "rest" << std::endl;
		while(true){
			const int cell_to_collapse_idx = cell_to_collapse();
			//std::cout << "found cell to collapse with idx " << cell_to_collapse_idx << std::endl;
			if(cell_to_collapse_idx == -1){
				break;
			}
			collapse_cell(cell_to_collapse_idx);
		}
		clear_anomalies();

		std::vector<char> result;
		result.reserve(size * size);

		for(const auto &cell: wave_map){
			result.push_back(cell.vals.front());
		}
		return result;
	}

	auto WaveFuncMap::log() -> void {
		for(int i = 0; i < size; i++){
			for(int j = 0; j < size; j++){
				const int idx = j + i * size;
				std::cout << wave_map.at(idx).vals.front() << ',';
			}
			std::cout << std::endl;
		}
	}
	auto WaveFuncMap::reset() -> void {
		wave_map.clear();
		for(int i = 0; i < size; i++){
			for(int j = 0; j < size; j++){
				const int idx = j + i * size;
				wave_map.push_back(Cell());
				
				wave_map.at(idx).id = idx;
				wave_map.at(idx).collapsed = false;
				wave_map.at(idx).entropy = 5;
				wave_map.at(idx).vals = {' ','+','|','-','#'};
			}
		}
	}
	auto WaveFuncMap::cell_to_collapse() -> int {
		//std::cout << "finding cell to collapse" << std::endl;
		std::vector<std::pair<int,int>> entropies;

		for(const auto &cell : wave_map){
			if(!cell.collapsed){
				entropies.push_back(std::make_pair(cell.id, cell.entropy));
			}
		}
		if(entropies.size() == 0){
			return -1;
		}
		std::sort(entropies.begin(), entropies.end(), entropy_compare);

		//last_item is goint to be equal to the last item that has the same entropy + 1
		const int least_entropy = entropies.at(0).second;
		const int list_size = entropies.size();
		int last_item = 0;
		while(last_item < list_size && entropies.at(last_item).second == least_entropy){
			last_item++;
		}
		//std::cout << "last item val is " << last_item << std::endl;
		//returns a random cell amongst the one's with least entropy
		return entropies.at(rand() % last_item).first;
	}
	auto WaveFuncMap::propagate(int idx) -> void {
		const auto neighbors = get_neighbors(idx);
		for(int i = 0; i < (int)neighbors.size(); i++){
			const int neighbor_idx = neighbors.at(i);
			if(neighbor_idx == -1 || wave_map.at(neighbor_idx).collapsed){
				continue;
			}
			const char cell_val = wave_map.at(idx).vals.front();

			const auto &constrains = adjecency[cell_val].at(i);

			std::vector<char> valid_tiles;

			std::copy_if(
				wave_map.at(neighbor_idx).vals.begin(),
				wave_map.at(neighbor_idx).vals.end(),
				std::back_inserter(valid_tiles),
				[&](char tile){return constrains.count(tile) == 1;}
			);

			wave_map[neighbor_idx].vals = valid_tiles;
			wave_map[neighbor_idx].entropy = valid_tiles.size();
			
			const int new_entropy = valid_tiles.size();
			if(new_entropy <= 1){
				wave_map[neighbor_idx].collapsed = true;
				//no possibilities for that space, put a '!' as a wildcard so that the sistem can progress
				if(new_entropy == 0){
					wave_map[neighbor_idx].vals = {'!'};
				}
				propagate(neighbor_idx);
			}
		}
	}

	auto WaveFuncMap::collapse_cell(int idx) -> void {
		//std::cout << "cell to collapse with idx " << idx << std::endl;
		int tile_pos = rand() % wave_map.at(idx).entropy;

		const char tile = wave_map[idx].vals.at(tile_pos);;

		wave_map[idx].collapsed = true;
		wave_map[idx].entropy = 1;
		wave_map[idx].vals = {tile};
		propagate(idx);
	}
	auto WaveFuncMap::get_neighbors(int idx) -> std::vector<int> {
		const int x = idx % size;
		const int y = (idx - x) / size;
		std::vector<int> res(4,-1);

		//top
		if(y > 0){
			res.at(0) = (x + (y - 1) * size);
		}
		//right
		if(x < size - 1){
			res.at(1) = ((x + 1) + size * y);
		}
		//bottom
		if(y < size - 1){
			res.at(2) = (x + (y + 1) * size);
		}
		//right
		if(x > 0){
			res.at(3) = ((x - 1) + size * y);
		}
		return res;
	}

	auto WaveFuncMap::place_end_point() -> void {
		//jeito unga bunga de fazer
		int rand_pos = rand() % (size * size);
		const int tries = 10;
		for(int i = 0; i < tries; i++){
			if(!wave_map.at(rand_pos).collapsed){
				break;
			}
			rand_pos = rand() % (size * size);
		}
		wave_map.at(rand_pos).collapsed = true;
		wave_map.at(rand_pos).entropy = 1;
		wave_map.at(rand_pos).vals = {'C'};
		propagate(rand_pos);
	}
	auto WaveFuncMap::clear_anomalies() -> void {
		for(int i = 0; i < size; i++){
			for(int j = 0; j < size; j++){
				const int idx = j + i * size;
				if(wave_map.at(idx).vals.front() == '!'){
					wave_map.at(idx).vals = {'+'};
				}
			}
		}
	}

	auto WaveFuncMap::print_adjecency_list() -> void {
		for(const auto &pair: adjecency){
			std::cout << '\'' << pair.first << '\'' << " : " << std::endl;
			for(int i = 0; i < 4; i++){
				std::cout << '\t' << i << " : ";
				for(auto tile: pair.second.at(i)){
					std::cout << '\'' << tile << '\'' << ", ";
				}
				std::cout << std::endl;
			}
		}
	}
}

