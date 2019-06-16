#include "Button.h"
#include <iostream>

sf::Font Button::s_font;

void Button::LoadFont(const std::string & path)
{
	if (!s_font.loadFromFile(path))
	{
		std::cout << "AJAJAJ!";
	}
}

Button::Button(const sf::Vector2f & position, const sf::Vector2f & size, bool adaptiveSize)
{
	m_shape.setPosition(position);
	m_shape.setSize(size);
	m_size = size;
	m_position = position;
	m_adaptive = adaptiveSize;
	m_text.setString(m_string);
	m_update = true;
	m_text.setFont(s_font);
}

Button::~Button()
{
}

void Button::SetPosition(const sf::Vector2f & position)
{
	m_shape.setPosition(position);
	m_position = position;
	m_update = true;
}

void Button::SetSize(const sf::Vector2f & size)
{
	m_shape.setSize(size);
	m_size = size;
	m_update = true;
}

void Button::SetOutlineSize(float size)
{
	m_shape.setOutlineThickness(size);
}

void Button::SetFillColor(const sf::Color & color)
{
	m_shape.setFillColor(color);
}

void Button::SetOutlineColor(const sf::Color & color)
{
	m_shape.setOutlineColor(color);
}

void Button::SetTexture(sf::Texture * texture)
{
	m_texture = texture;
	m_shape.setTexture(m_texture);
}

void Button::SetString(const std::wstring & string)
{
	m_string = string;
	m_text.setString(m_string);
	m_update = true;
}

void Button::SetAdaptive(bool adapt)
{
	m_adaptive = adapt;
	m_update = true;
}

void Button::SetTextSize(float size)
{
	m_text.setCharacterSize(size);
	m_update = true;
}

void Button::SetTextFillColor(const sf::Color & color)
{
	m_text.setFillColor(color);
}

void Button::SetTextOutlineColor(const sf::Color & color)
{
	m_text.setOutlineColor(color);
}

void Button::SetTextOutlineSize(float size)
{
	m_text.setOutlineThickness(size);
}

const sf::Vector2f & Button::GetPosition() const
{
	return m_shape.getPosition();
}

const sf::Vector2f & Button::GetSize() const
{
	return m_shape.getSize();
}

const sf::Color & Button::GetFillColor() const
{
	return m_shape.getFillColor();
}

const sf::Color & Button::GetOutlineColor() const
{
	return m_shape.getOutlineColor();
}

const sf::Texture * Button::GetTexture() const
{
	return m_texture;
}

const std::wstring & Button::GetString() const
{
	return m_string;
}

bool Button::PointInside(const sf::Vector2f & point) const
{
	return m_shape.getGlobalBounds().contains(point);	
}

void Button::Draw(sf::RenderWindow * wnd)
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

	wnd->draw(m_shape);
	wnd->draw(m_text);
}

void Button::_adaptSize(sf::RenderWindow * wnd)
{
	sf::Vector2f wndSize = (sf::Vector2f)wnd->getSize();
	m_shape.setPosition(wndSize.x * m_position.x, wndSize.y * m_position.y);
	m_shape.setSize(sf::Vector2f(wndSize.x * m_size.x, wndSize.y * m_size.y));
}

void Button::_update()
{
	bool hasPopped = false;
	m_text.setString(m_string);
	float textwidth = m_text.getLocalBounds().width;
	float texth = m_text.getLocalBounds().height;

	if (m_shape.getSize().y < texth)
		m_shape.setSize(sf::Vector2f(m_shape.getSize().x, texth));

	while (textwidth > m_shape.getSize().x)
	{
		std::string str = m_text.getString();
		str.pop_back();
		m_text.setString(str);
		hasPopped = true;
		textwidth = m_text.getLocalBounds().width;
	}

	if (hasPopped)
	{
		std::string str = m_text.getString();
		for (size_t i = str.size() - 1; i > str.size() - 4; --i)
		{
			str[i] = '.';
		}
		m_text.setString(str);
	}

	float cx = m_text.getLocalBounds().width * 0.5f;
	float cy = m_text.getLocalBounds().height * 0.5f;

	m_text.setOrigin(cx, cy);

	m_text.setPosition(m_shape.getPosition() + m_shape.getSize() * 0.5f);
	m_update = false;
}
