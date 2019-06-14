#pragma once
#include "Button.h"
#include <map>

class List
{
public:
	struct Item
	{
		Item(const std::string & _path = "")
		{
			path = _path;
		}

		Button Sprite;
		Button Remove;
		std::string path;
	};
public:
	List(bool adaptive = false);
	~List();

	void SetRect(float x, float y, float width, float height);
	void SetAdaptive(bool adaptive = true);

	void AddItem(const std::string & path);

	const std::vector<Item>* GetVector() const;

	const Item & GetItem(int index) const;
	int GetItemIndexAt(const sf::Vector2f & point);
	void EreaseItemAt(int index);
	void Scroll(int dir);
	bool PointInside(const sf::Vector2f & point);
	int GetItemsSize() const;
	void Draw(sf::RenderWindow * wnd);

private:
	void _adaptSize(sf::RenderWindow * wnd);
	void _update();

private:
	static const int CHARACTER_SIZE;

	std::vector<Item> m_items;
	sf::Vector2f m_sizeScale;
	sf::Vector2f m_positionScale;
	sf::Vector2f m_size;
	sf::Vector2f m_position;
	sf::Texture m_cross;

	sf::Vector2u m_wndSizeLastFrame = sf::Vector2u(0, 0);
	bool m_adaptive = false;
	bool m_update = false;

	int m_currScroll = 0;
	int m_scrollMax = 0;

	int m_drawStart = 0;
	int m_drawEnd = 0;

};