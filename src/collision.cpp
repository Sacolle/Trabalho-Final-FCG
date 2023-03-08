#include "collision.hpp"

#include <iostream>

namespace utils{
	SpacialHash::SpacialHash(float cell_x, float cell_y):
		cell_x(cell_x), cell_y(cell_y){}

	auto SpacialHash::get_quadrant(std::pair<int,int> key) -> std::vector<std::forward_list<Entt>*>{
		std::vector<std::forward_list<Entt>*> out;
		for(int i = -1; i < 2; i++){
			for(int j = -1; j < 2; j++){
				auto local = std::make_pair(key.first + i, key.second + j);
				auto list = get_cell(local);
				if(list != nullptr){
					out.push_back(list);
				}
			}
		}
		return out;
	}

	CollisionMap::CollisionMap(float max_width, float max_depth, float mover_cell_grain, float obj_cell_grain):
	max_width(max_width),max_depth(max_depth),mover_cell_grain(mover_cell_grain),obj_cell_grain(obj_cell_grain),
		mover_map(SpacialHash(max_width/mover_cell_grain, max_depth/mover_cell_grain)),
		obj_map(SpacialHash(max_width/obj_cell_grain, max_depth/obj_cell_grain)){}

	auto CollisionMap::insert_obj(Entt obj) -> int {
		auto c_key = obj_map.make_key(obj);
		obj_map.insert(c_key, obj);
		return 1;
	}
	auto CollisionMap::remove_obj(Entt obj) -> int {
		auto key = obj_map.make_key(obj);
		return mover_map.remove(key,obj);
	}
	//for the movable map
	auto CollisionMap::insert_mover(Entt entity) -> int {
		auto key = mover_map.make_key(entity);
		mover_map.insert(key,entity);
		return 1;
	}
	auto CollisionMap::remove_mover(Entt entity) -> int {
		auto key = mover_map.make_key(entity);
		return mover_map.remove(key,entity);
	}
	auto CollisionMap::box_to_box_collision(Entt target, Entt geometry, const glm::vec4 future_pos) const -> bool{

		const auto geo_cords = geometry->get_cords();
		const float box_x = geometry->get_x_radius();
		const float box_z = geometry->get_z_radius();

		const float x_radius = target->get_x_radius();
		const float z_radius = target->get_z_radius();

		//solution taken from stackoverflow https://stackoverflow.com/a/62852710
		return std::max(geo_cords.x - box_x, future_pos.x - x_radius) < std::min(geo_cords.x + box_x, future_pos.x + x_radius) &&
		std::max(geo_cords.z - box_z, future_pos.z - z_radius) < std::min(geo_cords.z + box_z, future_pos.z + z_radius);
	}
	auto CollisionMap::cilinder_to_box_collision(Entt target, Entt geometry, const glm::vec4 future_pos) const -> bool{
		const auto center2geometry = geometry->get_cords() - future_pos;

		const float x_radius = target->get_x_radius();
		const float z_radius = target->get_z_radius();
		
		const float co = center2geometry.z;
		const float ca = center2geometry.x;
		const float hipotenuse = sqrtf(co*co + ca*ca);
		
		const float rx = x_radius * (ca / hipotenuse);
		const float rz = z_radius * (co / hipotenuse);

		const glm::vec4 boundry_point(future_pos.x + rx, future_pos.y, future_pos.z + rz, 1);

		//std::cout << "cos(" << ca << "/" << hipotenuse << ") = " << cosf(ca/hipotenuse) << std::endl;
		//std::cout << "rx: " << rx << " = " << x_radius << "*" << cosf(ca/hipotenuse) << std::endl;
		//std::cout << "Boundry Point: " << boundry_point.x << " " << boundry_point.z << std::endl;

		const auto geo_cords = geometry->get_cords();
		const float box_x = geometry->get_x_radius();
		const float box_z = geometry->get_z_radius();

		return std::max(geo_cords.x - box_x, boundry_point.x) == std::min(geo_cords.x + box_x, boundry_point.x) &&
			   std::max(geo_cords.z - box_z, boundry_point.z) == std::min(geo_cords.z + box_z, boundry_point.z);
	}
	auto CollisionMap::box_to_cilinder_collision(Entt target, Entt geometry, const glm::vec4 future_pos) const -> bool{
		const float x_radius = target->get_x_radius();
		const float z_radius = target->get_z_radius();
		
		const auto geo_cords = geometry->get_cords();
		const auto geometry2center = future_pos - geo_cords;

		const float ca = geometry2center.x;
		const float co = geometry2center.z;
		const float hipotenuse = sqrtf(co*co + ca*ca);
		
		const float rx = x_radius * (ca / hipotenuse);
		const float rz = z_radius * (co / hipotenuse);

		const glm::vec4 geo_boundry_point(geo_cords.x + rx, geo_cords.y, geo_cords.z + rz, 1);

		return std::max(future_pos.x - x_radius, geo_boundry_point.x) == std::min(future_pos.x + x_radius, geo_boundry_point.x) &&
			   std::max(future_pos.z - z_radius, geo_boundry_point.z) == std::min(future_pos.z + z_radius, geo_boundry_point.z);
	}
	auto CollisionMap::cilinder_to_cilinder_collision(Entt target, Entt geometry, const glm::vec4 future_pos) const -> bool{
		const float x_radius = target->get_x_radius();
		const float z_radius = target->get_z_radius();
		
		const auto center2geometry = geometry->get_cords() - future_pos;
		
		const float co = center2geometry.z;
		const float ca = center2geometry.x;
		const float hipotenuse = sqrtf(co*co + ca*ca);
		
		const float rx = x_radius * (ca / hipotenuse);
		const float rz = z_radius * (co / hipotenuse);

		const glm::vec4 boundry_point(future_pos.x + rx, future_pos.y, future_pos.z + rz, 1);

		const auto point2geometry = geometry->get_cords() - boundry_point;
		const auto magnitude = sqrtf(point2geometry.x*point2geometry.x + point2geometry.z*point2geometry.z);

		return magnitude <= geometry->get_x_radius();
	}

	auto CollisionMap::direction_will_collide(Entt target, Entt geometry, const glm::vec4 &dir) -> Entt {
		const auto future_pos = target->get_cords() + (dir * target->get_speed());

		if(target->get_bbox_type() == entity::BBoxType::Rectangle){
			if(geometry->get_bbox_type() == entity::BBoxType::Rectangle){
				return box_to_box_collision(target, geometry, future_pos) ? geometry : nullptr;
			}else{
				return box_to_cilinder_collision(target, geometry, future_pos) ? geometry : nullptr;
			}
		}else{ 
			if(geometry->get_bbox_type() == entity::BBoxType::Rectangle){
				return cilinder_to_box_collision(target, geometry, future_pos) ? geometry : nullptr;
			}else{
				return cilinder_to_cilinder_collision(target, geometry, future_pos) ? geometry : nullptr;
			}
		}
	}
	auto CollisionMap::colide_direction(Entt entity, const glm::vec4 direction) -> std::shared_ptr<entity::Entity> {
		auto entity_map_key = mover_map.make_key(entity);
		auto mover_neighbors = mover_map.get_quadrant(entity_map_key);
		for(auto prox: mover_neighbors){
			for(auto it = prox->begin(); it != prox->end();++it){
				auto collision_target = direction_will_collide(entity, *it, direction);
				if(collision_target != nullptr){
					return collision_target;
				}
			}
		}
		auto obj_map_key = obj_map.make_key(entity);
		auto obj_neighbors = obj_map.get_quadrant(obj_map_key);
		for(auto prox: obj_neighbors){
			for(auto it = prox->begin(); it != prox->end();++it){
				auto collision_target = direction_will_collide(entity, *it, direction);
				if(collision_target != nullptr){
					return collision_target;
				}
			}
		}
		return nullptr;
	}
	/*
	auto CollisionMap::make_path(Entt entity, Entt player) -> Path {

	}*/
}