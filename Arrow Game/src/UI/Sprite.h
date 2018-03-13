#pragma once

#include "..\Global.h"

namespace ui
{
	class Sprite : public sf::Sprite
	{
	private:
		std::function<void(ui::Sprite *self)> m_updateFunction;
		bool m_hasCustomUpdateFunction = false;

	public:
		Sprite(const std::string &id, const sf::Vector2f &position);

		void Update(const sf::RenderWindow &window);
		void Draw(sf::RenderWindow &window);

		// Set
		void setUpdateFunction(const std::function<void(ui::Sprite *self)> &function);

		// Variables
		const std::string id;
	};
}