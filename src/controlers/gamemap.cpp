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
		reset();
		for(int i = 0; i < number_end_points; i++){
			place_end_point();
		}
		while(true){
			const int cell_to_collapse_idx = cell_to_collapse();
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
		int last_item = 0;
		while(entropies.at(last_item).second == least_entropy){
			last_item++;
		}
		//returns a random cell amongst the one's with least entropy
		return entropies.at(rand() % last_item).first;
	}
	auto WaveFuncMap::propagate(int idx) -> void {
		const auto neighbors = get_neighbors(idx);
		for(int i = 0; i < neighbors.size(); i++){
			const int neighbor_idx = neighbors.at(i);
			if(neighbor_idx == -1 || wave_map.at(neighbor_idx).collapsed){
				continue;
			}
			const char cell_val = wave_map.at(idx).vals.front();

			const auto &constrains = adjecency[cell_val].at(i);

			//goes throgh all the possible values for the tile
			const int neighbor_entropy = wave_map.at(neighbor_idx).vals.size();
			for(int j = 0; j < neighbor_entropy; j++){
				//removes the first element and stores its value
				const char elem = wave_map[neighbor_idx].vals.front();
				wave_map[neighbor_idx].vals.pop_front();
				//if it does not exist in the constrain values, add it back
				if(constrains.count(elem) == 0){
					wave_map[neighbor_idx].vals.push_back(elem);
				}
			}
			const int new_entropy = wave_map[neighbor_idx].vals.size();
			wave_map[neighbor_idx].entropy = new_entropy;
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
		int tile_pos = rand() % wave_map.at(idx).entropy;
		for(int i = 0; i < tile_pos; i++){
			wave_map[idx].vals.pop_front();
		}
		const char tile = wave_map[idx].vals.front();

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
}

