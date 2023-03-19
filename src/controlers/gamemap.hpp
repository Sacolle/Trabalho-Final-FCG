#pragma once

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <memory>

namespace controler{
	typedef struct cell{
		int id;
		bool collapsed;
		int entropy;
		std::vector<char> vals;
	} Cell;

	class WaveFuncMap{
		public:
			WaveFuncMap(int _size, int number_end_points);
			WaveFuncMap();
			~WaveFuncMap();

			auto generate() -> std::vector<char>;

			auto log() -> void;
			auto print_adjecency_list() -> void;

			inline auto set_size(int _size) -> void {size = _size;}
			inline auto set_number_end_points(int n) -> void {number_end_points = n;}

		private:
			auto reset() -> void;
			auto cell_to_collapse() -> int;
			auto propagate(int idx) -> void;
			auto collapse_cell(int idx) -> void;
			auto get_neighbors(int idx) -> std::vector<int>;


			auto place_end_point() -> void;
			auto clear_anomalies() -> void;

		 	int size;
			int number_end_points;

			std::vector<Cell> wave_map;

			std::unordered_map<char,std::vector<std::unordered_set<char>>> adjecency = {
				{' ',  {
					{' ','-','#'}, //up
					{' ','|','#'}, //right
					{' ','-','#'}, //down
					{' ','|','#'}  //left
				}},
				{'+', {
					{'|'},
					{'-'},
					{'|'},
					{'-'}
				}},
				{'|', {
					{'|','+'},
					{' ','#'},
					{'|','+'},
					{' ','#'}
				}},
				{'-', {
					{' ','#'},
					{'-','+'},
					{' ','#'},
					{'-','+'}
				}},
				{'#', {
					{' ','#','-'},
					{' ','#','|'},
					{' ','#','-'},
					{' ','#','|'}
				}},
				{'C', {
					{'|'},
					{'-'},
					{'|'},
					{'-'}
				}},
				{'!', {
					{' ','+','|','-','#'},
					{' ','+','|','-','#'},
					{' ','+','|','-','#'},
					{' ','+','|','-','#'}
				}}
			};
	};
}