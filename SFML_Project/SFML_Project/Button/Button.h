#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button
{
private:
	static sf::Font s_font;
	
public:
	static void LoadFont(const std::string & path);

public:
	Button(const sf::Vector2f & position = sf::Vector2f(0, 0), const sf::Vector2f & size = sf::Vector2f(200, 100), bool adaptiveSize = false);
	~Button();

	void SetPosition(const sf::Vector2f &position);
	void SetSize(const sf::Vector2f & size);
	void SetOutlineSize(float size);
	void SetFillColor(const sf::Color & color);
	void SetOutlineColor(const sf::Color & color);
	void SetTexture(sf::Texture * texture);
	void SetAdaptive(bool adapt = true);

	void SetString(const std::wstring & string);
	void SetTextSize(float size);
	void SetTextFillColor(const sf::Color & color);
	void SetTextOutlineColor(const sf::Color & color);
	void SetTextOutlineSize(float size);

	const sf::Vector2f & GetPosition() const;
	const sf::Vector2f & GetSize() const;
	const sf::Color & GetFillColor() const;
	const sf::Color & GetOutlineColor() const;
	const sf::Texture * GetTexture() const;
	const std::wstring & GetString() const;

	bool PointInside(const sf::Vector2f &point) const;
	void Draw(sf::RenderWindow * wnd);

private:
	void _adaptSize(sf::RenderWindow * wnd);
	void _update();

private:
	sf::Vector2f m_size;
	sf::Vector2f m_position;
	sf::Vector2u m_wndSizeLastFrame = sf::Vector2u(0,0);
	sf::RectangleShape m_shape;
	sf::Texture * m_texture = nullptr;
	sf::Text m_text;

	std::wstring m_string = L"";
	bool m_adaptive = false;
	bool m_update = false;
};