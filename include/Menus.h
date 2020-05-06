#pragma once
#include <Drawables.h>
#include <opencv2/opencv.hpp>

/*
class Sidebar : public Drawable
{
public:
	Sidebar(cv::Rect openpanelrect, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

	void ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname) override;

	bool CheckSelected(int index);
	void SelectDrawable(int index);

private:
	cv::Rect openPanelRect;
	vector<unique_ptr<Drawable>> menus;
	int selectedIndex = 1;
	//Drawable selectedDrawable;

	//CalibrationMenu calibMenu;
	//ProjectileMenu projMenu;

	bool ReturnTrue();
};



class CalibrationMenu : public Drawable
{
public:
	CalibrationMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};


class ProjectileMenu : public Drawable
{
public:
	ProjectileMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};
*/