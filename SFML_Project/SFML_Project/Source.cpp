#include <SFML/Graphics.hpp>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <Commdlg.h>
#include <shlobj_core.h>

#include "Button/List.h"
#include "Utility/Timer.h"

enum ButtonType
{
	ImportLocation = 0,
	ExportLocation = 1,
	ExportSelected = 2
};


void Run(sf::RenderWindow * wnd);

struct ScrollEvent
{
	bool scroll = false;
	int delta = 0;
};

ScrollEvent g_scrollEvent;
int main()
{
	UINT width = 1280;
	UINT height = 720;

	std::ifstream settings;
	settings.open("Data/Settings.txt");
	if (!settings)
	{
		std::ofstream createSettings;
		createSettings.open("Data/Settings.txt");
		createSettings << width << "\n";
		createSettings << height << "\n";
		createSettings.close();
		settings.open("Data/Settings.txt");
	}

	settings >> width;
	settings >> height;
	settings.close();
	sf::RenderWindow window(sf::VideoMode(width, height), "Media Sorter");

	window.setActive(false);
	std::thread eventPollThread(&Run, &window);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, width = event.size.width, height = event.size.height);
				window.setView(sf::View(visibleArea));
			}
			else if (event.type == sf::Event::MouseWheelMoved)
			{
				g_scrollEvent.scroll = true;
				g_scrollEvent.delta = event.mouseWheel.delta * -1;
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window.close();

	}
	eventPollThread.join();

	std::ofstream createSettings;
	createSettings.open("Data/Settings.txt");
	createSettings << width << "\n";
	createSettings << height << "\n";
	createSettings.close();

	return 0;
}

void Run(sf::RenderWindow * wnd)
{
	Button::LoadFont("Data/ArialUnicodeMS.ttf");

	sf::Color ambient(128, 128, 128);
	sf::Color hover(255, 255, 255);
	sf::Color press(64, 64, 64);

	const UINT NUMBER_OF_BUTTONS = 3;

	std::vector<Button> buttons(NUMBER_OF_BUTTONS);
	Button exportPathAsButton;
	{
		buttons[ImportLocation].SetPosition(sf::Vector2f(0,0));
		buttons[ImportLocation].SetSize(sf::Vector2f(0.2, 0.2));
		buttons[ImportLocation].SetAdaptive(true);
		buttons[ImportLocation].SetFillColor(sf::Color(128, 128, 128));
		buttons[ImportLocation].SetString("Add Media Location");
		buttons[ImportLocation].SetTextFillColor(sf::Color::Black);
		buttons[ImportLocation].SetTextSize(16);
		buttons[ImportLocation].SetOutlineSize(-2);
		buttons[ImportLocation].SetOutlineColor(sf::Color::Black);

		buttons[ExportLocation].SetPosition(sf::Vector2f(0.8, 0));
		buttons[ExportLocation].SetSize(sf::Vector2f(0.2, 0.2));
		buttons[ExportLocation].SetAdaptive(true);
		buttons[ExportLocation].SetFillColor(sf::Color(128, 128, 128));
		buttons[ExportLocation].SetString("Set Export Location");
		buttons[ExportLocation].SetTextFillColor(sf::Color::Black);
		buttons[ExportLocation].SetTextSize(16);
		buttons[ExportLocation].SetOutlineSize(-2);
		buttons[ExportLocation].SetOutlineColor(sf::Color::Black);

		exportPathAsButton.SetPosition(sf::Vector2f(0.8, 0.2));
		exportPathAsButton.SetSize(sf::Vector2f(0.2, 0.2));
		exportPathAsButton.SetAdaptive(true);
		exportPathAsButton.SetFillColor(sf::Color::Transparent);
		exportPathAsButton.SetTextFillColor(sf::Color::Black);
		exportPathAsButton.SetTextSize(16);

		buttons[ExportSelected].SetPosition(sf::Vector2f(0.8, 0.8));
		buttons[ExportSelected].SetSize(sf::Vector2f(0.2, 0.2));
		buttons[ExportSelected].SetAdaptive(true);
		buttons[ExportSelected].SetFillColor(sf::Color(128, 128, 128));
		buttons[ExportSelected].SetString("Exported Selected Media");
		buttons[ExportSelected].SetTextFillColor(sf::Color::Black);
		buttons[ExportSelected].SetTextSize(16);
		buttons[ExportSelected].SetOutlineSize(-2);
		buttons[ExportSelected].SetOutlineColor(sf::Color::Black);
	}


	std::string exportPath = "";
	List list(true);

	list.SetRect(0.21, 0.0, 0.58, 1.0f);

	bool MousePressedLastFrame = false;

	while (wnd->isOpen())
	{
		sf::Vector2f mp = (sf::Vector2f)sf::Mouse::getPosition(*wnd);
		
		bool mousePress = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		bool mouseReleased = !mousePress && MousePressedLastFrame;

		if (list.PointInside(mp))
		{
			if (g_scrollEvent.scroll)
			{
				list.Scroll(g_scrollEvent.delta);
				g_scrollEvent.scroll = false;
			}

			if (mouseReleased)
			{
				int index = list.GetItemIndexAt(mp);
				if (index >= 0)
				{
					List::Item item = list.GetItem(index);
					if (item.Remove.PointInside(mp))
					{
						list.EreaseItemAt(index);
					}
				}
			}
		}

		wnd->clear(sf::Color::White);
		for (int i = 0; i < NUMBER_OF_BUTTONS; ++i)
		{
			if (buttons[i].PointInside(mp))
			{
				if (mouseReleased)
				{
					ButtonType bt = (ButtonType)i;
					
					switch (bt)
					{
					case ImportLocation:
					{
						std::cout << "Open Import stuff\n";
						LPITEMIDLIST lpItem = NULL;
						bool first = true;
						while (lpItem != NULL || first)
						{
							first = false;
							TCHAR szDir[999];
							BROWSEINFO bInfo;
							ZeroMemory(&bInfo, sizeof(bInfo));
							bInfo.hwndOwner = NULL;
							bInfo.pidlRoot = NULL;
							bInfo.pszDisplayName = szDir;
							bInfo.lpszTitle = "Please, select a folder";
							bInfo.ulFlags = 0;
							bInfo.lpfn = NULL;
							bInfo.lParam = 0;
							bInfo.iImage = -1;
							lpItem = SHBrowseForFolder(&bInfo);
							if (lpItem != NULL)
							{
								SHGetPathFromIDList(lpItem, szDir);
								//MessageBox(NULL, szDir, "File Name", MB_OK);
								list.AddItem(std::string(szDir));
							}
						}
					}
						break;
					case ExportLocation:
						std::cout << "Open Export stuff\n";
						{
							LPITEMIDLIST lpItem = NULL;
							TCHAR szDir[999];
							BROWSEINFO bInfo;
							ZeroMemory(&bInfo, sizeof(bInfo));
							bInfo.hwndOwner = NULL;
							bInfo.pidlRoot = NULL;
							bInfo.pszDisplayName = szDir;
							bInfo.lpszTitle = "Please, select a folder";
							bInfo.ulFlags = 0;
							bInfo.lpfn = NULL;
							bInfo.lParam = 0;
							bInfo.iImage = -1;
							lpItem = SHBrowseForFolder(&bInfo);
							if (lpItem != NULL)
							{
								SHGetPathFromIDList(lpItem, szDir);
								
								exportPath = std::string(szDir);
								exportPathAsButton.SetString(exportPath);
							}
							
						}
						break;
					case ExportSelected:
						std::cout << "Export stuff\n";
						{
							if (exportPath != "")
							{
								// Export everything
							}
							else
							{
								MessageBox(NULL, "You need to select an export path", "Error", MB_OK);
							}

						}
						break;
					}
				}
				else if (mousePress)
					buttons[i].SetFillColor(press);
				else
					buttons[i].SetFillColor(hover);
			}
			else
			{
				buttons[i].SetFillColor(ambient);
			}

			buttons[i].Draw(wnd);
		}
		list.Draw(wnd);
		exportPathAsButton.Draw(wnd);
		wnd->display();

		MousePressedLastFrame = mousePress;
	}
}
