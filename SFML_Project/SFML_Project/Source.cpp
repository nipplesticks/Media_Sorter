#define _CRT_SECURE_NO_WARNINGS

#define NOMINMAX
#include "Button/List.h"
#include "Utility/Timer.h"
#undef NOMINMAX

#include <Windows.h>
#include <string>
#include <iostream>
#include <conio.h>
#include <exception>
#include <ostream>
#include <stdexcept>
#include <ShlObj.h>
#include <stdlib.h>
#include <thread>
#include <fstream>
#include <vector>
#include <Commdlg.h>
#include <shlobj_core.h>
#include <sys/stat.h>
#include <filesystem>
#include <sstream>

using namespace std;
#include <gdiplus.h>
#include <stdio.h>
#pragma comment (lib,"Gdiplus.lib")

#include "Utility/Timer.h"

using namespace Gdiplus;
namespace fs = std::experimental::filesystem;

enum ButtonType
{
	ImportLocation = 0,
	ExportLocation = 1,
	ExportSelected = 2
};


void Run(sf::RenderWindow * wnd);
void Export(const std::wstring exportPath, std::vector<std::wstring> folders);
void ExportPictures(const std::map<std::wstring, int>& pictures, const std::wstring & directory, sf::RenderWindow* wnd, std::map<std::wstring, int>& other);
void ExportOther(const std::map<std::wstring, int>& other, const std::wstring& directory, sf::RenderWindow* wnd);
std::wstring GetFileType(const std::wstring& path);
std::wstring GetNumberAsMonth(const std::wstring& num);
std::wstring strToWchar(const std::wstring& str)
{
	return std::wstring(str.begin(), str.end());
}

struct ScrollEvent
{
	bool scroll = false;
	int delta = 0;
};

ScrollEvent g_scrollEvent;

int g_totalFiles = 0;
int g_currentFile = 0;
bool g_done = true;
const int LOADING_WINDOW_SIZE_X = 400;
const int LOADING_WINDOW_SIZE_Y = 200;
Button g_loadBck;
Button g_loadBar;

int main()
{
#ifndef _DEBUG
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
#endif
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	
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
			if (event.type == sf::Event::Closed && g_done)
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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && g_done)
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
	Button mediaLocation;
	Button listBackground;
	{
		buttons[ImportLocation].SetPosition(sf::Vector2f(0,0));
		buttons[ImportLocation].SetSize(sf::Vector2f(0.2, 0.2));
		buttons[ImportLocation].SetAdaptive(true);
		buttons[ImportLocation].SetFillColor(sf::Color(128, 128, 128));
		buttons[ImportLocation].SetString(L"Add Media Location");
		buttons[ImportLocation].SetTextFillColor(sf::Color::Black);
		buttons[ImportLocation].SetTextSize(16);
		buttons[ImportLocation].SetOutlineSize(-2);
		buttons[ImportLocation].SetOutlineColor(sf::Color::Black);

		buttons[ExportLocation].SetPosition(sf::Vector2f(0.8, 0));
		buttons[ExportLocation].SetSize(sf::Vector2f(0.2, 0.2));
		buttons[ExportLocation].SetAdaptive(true);
		buttons[ExportLocation].SetFillColor(sf::Color(128, 128, 128));
		buttons[ExportLocation].SetString(L"Set Export Location");
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

		mediaLocation.SetPosition(sf::Vector2f(0.4, 0.0));
		mediaLocation.SetSize(sf::Vector2f(0.2, 0.05));
		mediaLocation.SetAdaptive(true);
		mediaLocation.SetFillColor(sf::Color::Transparent);
		mediaLocation.SetTextFillColor(sf::Color::Black);
		mediaLocation.SetTextSize(16);
		mediaLocation.SetOutlineSize(1);
		mediaLocation.SetOutlineColor(sf::Color::Black);
		mediaLocation.SetString(L"List of media location(s)");

		listBackground.SetPosition(sf::Vector2f(0.21, 0.0));
		listBackground.SetSize(sf::Vector2f(0.58, 1));
		listBackground.SetAdaptive(true);
		listBackground.SetFillColor(sf::Color(200, 200, 200));


		buttons[ExportSelected].SetPosition(sf::Vector2f(0.8, 0.8));
		buttons[ExportSelected].SetSize(sf::Vector2f(0.2, 0.2));
		buttons[ExportSelected].SetAdaptive(true);
		buttons[ExportSelected].SetFillColor(sf::Color(128, 128, 128));
		buttons[ExportSelected].SetString(L"Exported Selected Media");
		buttons[ExportSelected].SetTextFillColor(sf::Color::Black);
		buttons[ExportSelected].SetTextSize(16);
		buttons[ExportSelected].SetOutlineSize(-2);
		buttons[ExportSelected].SetOutlineColor(sf::Color::Black);
	}


	g_loadBck.SetAdaptive(false);
	g_loadBck.SetPosition(sf::Vector2f(50, 150));
	g_loadBck.SetSize(sf::Vector2f(300, 32));
	g_loadBck.SetFillColor(sf::Color::Transparent);
	g_loadBck.SetOutlineSize(2);
	g_loadBck.SetOutlineColor(sf::Color::Black);

	g_loadBar.SetAdaptive(false);
	g_loadBar.SetPosition(sf::Vector2f(50, 150));
	g_loadBar.SetSize(sf::Vector2f(0, 32));
	g_loadBar.SetFillColor(sf::Color::Green);

	std::wstring exportPath = L"";
	List list(true);

	list.SetRect(0.21, 0.05, 0.58, 0.8f);

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
		listBackground.Draw(wnd);

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
							WCHAR szDir[999];
							BROWSEINFO bInfo;
							ZeroMemory(&bInfo, sizeof(bInfo));
							bInfo.hwndOwner = NULL;
							bInfo.pidlRoot = NULL;
							bInfo.pszDisplayName = szDir;
							bInfo.lpszTitle = L"Select a media location";
							bInfo.ulFlags = 0;
							bInfo.lpfn = NULL;
							bInfo.lParam = 0;
							bInfo.iImage = -1;
							lpItem = SHBrowseForFolder(&bInfo);
							if (lpItem != NULL)
							{
								SHGetPathFromIDListW(lpItem, szDir);
								//MessageBox(NULL, szDir, "File Name", MB_OK);
								std::wstring str = std::wstring(szDir);
								list.AddItem(str);
							}
						}
					}
						break;
					case ExportLocation:
						std::cout << "Open Export stuff\n";
						{
							LPITEMIDLIST lpItem = NULL;
							WCHAR szDir[999];
							BROWSEINFO bInfo;
							ZeroMemory(&bInfo, sizeof(bInfo));
							bInfo.hwndOwner = NULL;
							bInfo.pidlRoot = NULL;
							bInfo.pszDisplayName = szDir;
							bInfo.lpszTitle = L"Please, select an export location";
							bInfo.ulFlags = 0;
							bInfo.lpfn = NULL;
							bInfo.lParam = 0;
							bInfo.iImage = -1;
							lpItem = SHBrowseForFolder(&bInfo);
							if (lpItem != NULL)
							{
								SHGetPathFromIDListW(lpItem, szDir);
								
								std::wstring str = std::wstring(szDir);
								exportPath = str;
								exportPathAsButton.SetString(exportPath);
							}
							
						}
						break;
					case ExportSelected:
						std::cout << "Export stuff\n";
						{
							const std::vector<List::Item>* itemList = list.GetVector();
							size_t itemListSize = itemList->size();
							if (exportPath != L"" && itemListSize > 0)
							{
								// Export everything
								std::vector<std::wstring> folders(itemListSize);
								for (size_t i = 0; i < itemListSize; i++)
								{
									folders[i] = itemList->at(i).path;
								}
								Export(exportPath, folders);
								for (size_t i = 0; i < itemListSize; i++)
								{
									list.EreaseItemAt(0);
								}
								g_done = true;
							}
							else if (exportPath == L"")
							{
								MessageBox(NULL, L"You need to select an export path", L"Error", MB_OK);
							}
							else
							{
								MessageBox(NULL, L"You need to select at least one media location", L"Error", MB_OK);
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
		mediaLocation.Draw(wnd);
		wnd->display();

		MousePressedLastFrame = mousePress;
	}
}

void StartProgress(const std::wstring exportPath, std::vector<std::wstring> folders, sf::RenderWindow * wnd)
{
	std::map<std::wstring, int> uniqueFolders;
	std::map<std::wstring, int> uniquePictures;
	std::map<std::wstring, int> uniqueOther;
	for (auto& f : folders)
	{
		if (uniqueFolders.find(f) == uniqueFolders.end())
		{
			uniqueFolders.insert(std::make_pair(f, 0));
		}
	}
	folders.clear();
	folders = std::vector<std::wstring>(uniqueFolders.size());
	UINT counter = 0;
	for (auto& d : uniqueFolders)
	{
		folders[counter++] = d.first;
	}

	Button parse;

	std::wstring str = L"Parsing files";
	std::wstring dot = L"";
	
	parse.SetAdaptive(false);
	parse.SetPosition(sf::Vector2f(0, 0));
	parse.SetSize(sf::Vector2f(LOADING_WINDOW_SIZE_X, LOADING_WINDOW_SIZE_Y));
	parse.SetFillColor(sf::Color::Transparent);
	parse.SetTextFillColor(sf::Color::Black);
	parse.SetTextSize(16);
	parse.SetString(L"Parsing files");

	int dotCounter = 0;
	counter = 0;
	Timer t;
	t.Start();
	double time = 0.0;
	for (auto& path : folders)
	{
		for (auto& p : fs::recursive_directory_iterator(path))
		{
			if (!wnd->isOpen())
				return;

			time += t.Stop();
			if (time > 1)
			{
				time -= 1;

				dotCounter++;
				dotCounter = dotCounter % 4;

				dot = L"";
				for (int i = 0; i < dotCounter; i++)
				{
					dot += L".";
				}
				parse.SetString(str + dot + L"\nNumber of files: " + std::to_wstring(uniquePictures.size() + uniqueOther.size()));
				wnd->clear(sf::Color::White);
				parse.Draw(wnd);
				wnd->display();
			}


			std::wstringstream ss;
			ss << p;
			std::wstring file(ss.str());
			Image* f = nullptr;
			std::wstring wstr = std::wstring(file.begin(), file.end());
			const wchar_t* cwstr = wstr.c_str();
			f = new Image(cwstr);

			if (f->GetType() != ImageType::ImageTypeUnknown)
			{
				//Is image
				uniquePictures.insert_or_assign(file, 0);
			}
			else
			{
				uniqueOther.insert_or_assign(file, 0);
			}
			if (f) delete f;
		}
	}

	g_totalFiles = uniquePictures.size() + uniqueOther.size();

	ExportPictures(uniquePictures, exportPath, wnd, uniqueOther);
	ExportOther(uniqueOther, exportPath, wnd);
}

void Export(const std::wstring exportPath, std::vector<std::wstring> folders)
{
	g_done = false;
	sf::RenderWindow wnd(sf::VideoMode(LOADING_WINDOW_SIZE_X, LOADING_WINDOW_SIZE_Y), "Sorting your media", sf::Style::Titlebar | sf::Style::Close);
	wnd.setActive(false);
	std::thread ex(&StartProgress, exportPath, folders, &wnd);

	while (wnd.isOpen() && !g_done)
	{
		sf::Event event;
		while (wnd.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				wnd.close();
				g_done = true;
			}
		}
	}



	ex.join();

	wnd.close();

}

void ExportPictures(const std::map<std::wstring, int>& pictures, const std::wstring& directory, sf::RenderWindow* wnd, std::map<std::wstring, int> & other)
{
	Timer t;
	t.Start();
	double time = 0.0;

	Button parse;
	parse.SetAdaptive(false);
	parse.SetPosition(sf::Vector2f(0, 0));
	parse.SetSize(sf::Vector2f(LOADING_WINDOW_SIZE_X, LOADING_WINDOW_SIZE_Y));
	parse.SetFillColor(sf::Color::Transparent);
	parse.SetTextFillColor(sf::Color::Black);
	parse.SetTextSize(16);
	
	std::wstring str = L"Sorting your media";
	std::wstring dot = L"";
	int dotCounter = 0;

	for (auto& pic : pictures)
	{
		if (!wnd->isOpen())
			break;
		g_currentFile++;

		time += t.Stop();

		if (time > 0.5)
		{
			time -= 0.5;

			float sizeX = ((double)g_currentFile / g_totalFiles) * 300.0;
		
			g_loadBar.SetSize(sf::Vector2f(sizeX, 32));
			dotCounter++;
			dotCounter = dotCounter % 4;

			dot = L"";
			for (int i = 0; i < dotCounter; i++)
			{
				dot += L".";
			}
			parse.SetString(str + dot);
			wnd->clear(sf::Color::White);
			parse.Draw(wnd);
			g_loadBck.Draw(wnd);
			g_loadBar.Draw(wnd);
			wnd->display();
		}


		std::wstring moveTo = directory + L"\\";
		std::wstring file = pic.first;

		Image* f = nullptr;

		std::wstring wstr = std::wstring(file.begin(), file.end());
		const wchar_t* cwstr = wstr.c_str();

		f = new Image(cwstr);

		UINT size = f->GetPropertyItemSize(PropertyTagExifDTOrig);
		PropertyItem* pi = nullptr;

		std::wstring dateTaken = L"0000-00-00 00.00.00";

		if (size != 0)
		{
			pi = (PropertyItem*)malloc(size);
			f->GetPropertyItem(PropertyTagExifDTOrig, size, pi);
			std::string tmp = static_cast<char*>(pi->value);
			dateTaken = std::wstring(tmp.begin(), tmp.end());
		}
		else
		{
			delete f;
			g_currentFile--;

			if (other.find(file) == other.end())
				other.insert(std::make_pair(file, 0));

			continue;
		}


		dateTaken[4] = dateTaken[7] = L'-';
		dateTaken[13] = dateTaken[16] = L'.';

		std::wstring newFileName = dateTaken;
		std::wstring fileType = GetFileType(file);
		
		delete f;
		f = nullptr;
		delete pi;
		pi = nullptr;

		std::wstring year = dateTaken.substr(0, 4);
		std::wstring month = GetNumberAsMonth(dateTaken.substr(5, 2));

		bool succeeded = false;
		int counter = 1;

		auto from = file;

		if (
			(CreateDirectoryW((moveTo + year).c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) &&
			(CreateDirectoryW((moveTo + year + L"\\" + month).c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
			)
		{
			moveTo += year + L"\\" + month + L"\\";
			auto to = (moveTo + newFileName + fileType).c_str();

			if (!CopyFileW(from.c_str(), to, TRUE))
			{
				while (!succeeded && counter <= 100)
				{
					std::wstring lol = moveTo + newFileName + L"_" + std::to_wstring(counter++) + fileType;
					succeeded = CopyFileW(from.c_str(), strToWchar(lol.c_str()).c_str(), TRUE);
				}
			}
		}
	}
}

void ExportOther(const std::map<std::wstring, int>& other, const std::wstring& directory, sf::RenderWindow* wnd)
{
	Timer t;
	t.Start();
	double time = 0.0;

	Button parse;
	parse.SetAdaptive(false);
	parse.SetPosition(sf::Vector2f(0, 0));
	parse.SetSize(sf::Vector2f(LOADING_WINDOW_SIZE_X, LOADING_WINDOW_SIZE_Y));
	parse.SetFillColor(sf::Color::Transparent);
	parse.SetTextFillColor(sf::Color::Black);
	parse.SetTextSize(16);

	std::wstring str = L"Sorting your media";
	std::wstring dot = L"";
	int dotCounter = 0;

	for (auto& o : other)
	{
		if (!wnd->isOpen())
			break;
		g_currentFile++;

		time += t.Stop();

		if (time > 0.5)
		{
			time -= 0.5;

			float sizeX = ((double)g_currentFile / g_totalFiles) * 300.0;

			g_loadBar.SetSize(sf::Vector2f(sizeX, 32));
			dotCounter++;
			dotCounter = dotCounter % 4;

			dot = L"";
			for (int i = 0; i < dotCounter; i++)
			{
				dot += L".";
			}
			parse.SetString(str + dot);
			wnd->clear(sf::Color::White);
			parse.Draw(wnd);
			g_loadBck.Draw(wnd);
			g_loadBar.Draw(wnd);
			wnd->display();
		}

		std::wstring moveTo = directory + L"\\";
		std::wstring file = o.first;
		if (true)
		{
			WIN32_FILE_ATTRIBUTE_DATA fInfo;
			ZeroMemory(&fInfo, sizeof(fInfo));
			
			
			if (GetFileAttributes(file.c_str()) == FILE_ATTRIBUTE_DIRECTORY)
				continue;

			if (GetFileAttributesExW(file.c_str(), GetFileExInfoStandard, &fInfo))
			{
				SYSTEMTIME systime;
				FILETIME localFTime;
				FileTimeToLocalFileTime(&fInfo.ftLastWriteTime, &localFTime);
				//FileTimeToSystemTime(&fInfo.ftLastWriteTime, &systime);
				FileTimeToSystemTime(&localFTime, &systime);
				std::wstring monthNumber = std::to_wstring(systime.wMonth);
				std::wstring dayNumber = std::to_wstring(systime.wDay);
				std::wstring hourNumber = std::to_wstring(systime.wHour);
				std::wstring minuteNumber = std::to_wstring(systime.wMinute);
				std::wstring secondNumber = std::to_wstring(systime.wSecond);
				if (monthNumber.size() == 1 || monthNumber.length() == 1)
				{
					monthNumber.insert(monthNumber.begin(), '0');
				}
				if (dayNumber.size() == 1 || dayNumber.length() == 1)
				{
					dayNumber.insert(dayNumber.begin(), '0');
				}
				if (hourNumber.size() == 1 || hourNumber.length() == 1)
				{
					hourNumber.insert(hourNumber.begin(), '0');
				}
				if (minuteNumber.size() == 1 || minuteNumber.length() == 1)
				{
					minuteNumber.insert(minuteNumber.begin(), '0');
				}
				if (secondNumber.size() == 1 || secondNumber.length() == 1)
				{
					secondNumber.insert(secondNumber.begin(), '0');
				}

				std::wstring year = std::to_wstring(systime.wYear), month = GetNumberAsMonth(monthNumber);
				std::wstring newName = year + L"-" + monthNumber + L"-" + dayNumber + L" " + hourNumber + L"." + minuteNumber + L"." + secondNumber;
				std::wstring fileType = GetFileType(file);

				bool succeeded = false;
				int counter = 1;

				auto from = file;
				std::wstring str1 = moveTo + year, str2 = moveTo + year + L"\\" + month;
				std::wstring if1(str1.begin(), str1.end()), if2(str2.begin(), str2.end());

				if (
					(CreateDirectoryW((moveTo + year).c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) &&
					(CreateDirectoryW((moveTo + year + L"\\" + month).c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
					)
				{
					moveTo += year + L"\\" + month + L"\\";

					auto to = (moveTo + newName + fileType).c_str();
					if (!CopyFileW(from.c_str(), to, TRUE))
					{
						while (!succeeded && counter <= 100)
						{
							std::wstring lol = moveTo + newName + L"_" + std::to_wstring(counter++) + fileType;
							succeeded = CopyFileW(from.c_str(), strToWchar(lol.c_str()).c_str(), TRUE);
						}
					}
				}

			}
		}
	}
	g_done = true;
}

std::wstring GetFileType(const std::wstring& path)
{
	int counter = (int)path.size() - 1;
	std::wstring type = L"";
	while (path[counter] != '.' && counter > -1)
		type += path[counter--];
	type += '.';

	std::reverse(type.begin(), type.end());

	return type;
}

std::wstring GetNumberAsMonth(const std::wstring& num)
{
	std::wstring month = num;

	int mNum = -1;
	if (num != L"")
	{
		//mNum = wcstol(num.c_str(), NULL, num.length());
		std::string nStr(num.begin(), num.end());
		mNum = atoi(nStr.c_str());
	}
	if (mNum > 0 && mNum < 13)
	{
		switch (mNum)
		{
		case 1:
			month = L"Jan";
			break;
		case 2:
			month = L"Feb";
			break;
		case 3:
			month = L"Mars";
			break;
		case 4:
			month = L"April";
			break;
		case 5:
			month = L"Maj";
			break;
		case 6:
			month = L"Juni";
			break;
		case 7:
			month = L"Juli";
			break;
		case 8:
			month = L"Aug";
			break;
		case 9:
			month = L"Sep";
			break;
		case 10:
			month = L"Okt";
			break;
		case 11:
			month = L"Nov";
			break;
		case 12:
			month = L"Dec";
			break;
		default:
			month = L"No_meta";
			break;
		}
	}

	return num + L"-" + month;
}
