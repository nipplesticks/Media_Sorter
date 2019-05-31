#include "List.h"

const int List::CHARACTER_SIZE = 16;

List::List(bool adaptive)
{
	m_adaptive = adaptive;

	m_cross.loadFromFile("Data/Cross.png");
}

List::~List()
{
}

void List::SetRect(float x, float y, float width, float height)
{
	m_sizeScale.x = width;
	m_sizeScale.y = height;
	m_positionScale.x = x;
	m_positionScale.y = y;

	m_size = m_sizeScale;
	m_position = m_positionScale;

	m_update = true;
}

void List::SetAdaptive(bool adaptive)
{
	m_adaptive = adaptive;
}

void List::AddItem(const std::string & path)
{
	Item item(path);
	item.Remove.SetTexture(&m_cross);
	item.Remove.SetAdaptive(false);
	item.Remove.SetSize(sf::Vector2f(16, 16));
	item.Sprite.SetAdaptive(false);
	item.Sprite.SetTextFillColor(sf::Color::Black);
	item.Sprite.SetString(item.path);
	item.Sprite.SetFillColor(sf::Color::Transparent);
	item.Sprite.SetTextSize(CHARACTER_SIZE);
	m_items.push_back(item);
	
	m_update = true;
}

const List::Item & List::GetItem(int index) const
{
	return m_items[index];
}

int List::GetItemIndexAt(const sf::Vector2f & point)
{

	for (int i = m_drawStart; i < m_drawEnd; ++i)
	{
		if (m_items[i].Sprite.PointInside(point))
		{
			return i;
		}
	}

	return -1;
}

void List::EreaseItemAt(int index)
{
	m_items.erase(m_items.begin() + index);
	m_update = true;
}

void List::Scroll(int dir)
{
	m_currScroll += dir;
	if (m_currScroll < 0)
		m_currScroll = 0;
	else if (m_currScroll > m_scrollMax)
		m_currScroll = m_scrollMax;
	m_update = true;
}

bool List::PointInside(const sf::Vector2f & point)
{
	return point.x > m_position.x && point.x < m_position.x + m_size.x &&
		point.y > m_position.y && point.y < m_position.y + m_size.y;
}

int List::GetItemsSize() const
{
	return m_items.size();
}

void List::Draw(sf::RenderWindow * wnd)
{
	if (wnd->getSize().x != m_wndSizeLastFrame.x || wnd->getSize().y != m_wndSizeLastFrame.y)
		m_update = true;
	m_wndSizeLastFrame = wnd->getSize();

	if (m_update)
	{
		if (m_adaptive)
			_adaptSize(wnd);
		_update();
	}

	sf::Vector2f listPos = m_position;

	for (int i = m_drawStart; i < m_drawEnd; ++i)
	{
		m_items[i].Sprite.SetPosition(listPos);
		m_items[i].Sprite.SetSize(sf::Vector2f(m_size.x, CHARACTER_SIZE));
		m_items[i].Sprite.Draw(wnd);
		m_items[i].Remove.SetPosition(listPos + sf::Vector2f(m_items[i].Sprite.GetSize().x, 0.0f) - sf::Vector2f(CHARACTER_SIZE, 0));
		m_items[i].Remove.Draw(wnd);
		listPos.y += CHARACTER_SIZE;
	}
}

void List::_adaptSize(sf::RenderWindow * wnd)
{
	sf::Vector2f wndSize = (sf::Vector2f)wnd->getSize();

	m_position = sf::Vector2f(wndSize.x * m_positionScale.x, wndSize.y * m_positionScale.y);
	m_size = sf::Vector2f(wndSize.x * m_sizeScale.x, wndSize.y * m_sizeScale.y);
}

void List::_update()
{
	int maxAmount = m_size.y / CHARACTER_SIZE;
	m_scrollMax = m_items.size() - maxAmount;
	m_scrollMax = std::max(m_scrollMax, 0);
	Scroll(0);
	m_drawStart = m_currScroll;
	m_drawEnd = m_drawStart + maxAmount;
	m_drawEnd = std::min(m_drawEnd, (int)m_items.size());
}
