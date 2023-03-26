#include "screen.hpp"


namespace entity {
	//calls its draw method and it's nodes draw methods
	auto Screen::draw_nodes() -> void {
		for(const auto &node : nodes){
			node->draw(node->get_transform());
		}
	}
	auto Screen::insert_nodes(std::shared_ptr<Node> noh, float b, float t, float l, float r, float screen_width, float screen_height) -> void {
		noh->set_bounds(b,t,l,r,screen_width,screen_height);
		nodes.push_back(noh);
	}
	auto Screen::click_action(double x, double y, float screen_width, float screen_height) -> MenuOptions {
		for(const auto &node : nodes){
			const float px = float(x)/screen_width;
			const float py = float(y)/screen_height;
			if(node->click_inside(px,py)){
				return node->get_action();
			}
		}
		return MenuOptions::None;
	}
	auto Node::click_inside(float x, float y) -> bool {
		return (x > left) && (x < right) && (y < bottom) && (y > top);
	}

	auto Node::set_bounds(float b, float t, float l, float r, float screen_width, float screen_height) -> void {
		bottom = b/screen_height;
		top = t/screen_height;
		left = l/screen_width;
		right = r/screen_width;
	}
}