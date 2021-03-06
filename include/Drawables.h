#pragma once

#include <opencv2/opencv.hpp>
#include <RecordingHelper.h>


using namespace std;

class Drawable
{
public:

	Drawable(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

	virtual void ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname);
	void Enable();
	void Disable();

	virtual void ProcessAllClicks(int x, int y, bool isdown); 
	vector < unique_ptr<Drawable>> children;
protected:

	const cv::Scalar TEXT_COLOR = cv::Scalar(255, 255, 255, 1);
	const cv::Scalar BACKGROUND_COLOR = cv::Scalar(70, 70, 70, 1);

	//static vector<Drawable> allEnabledDrawables;
	static vector<unique_ptr<Drawable>> allEnabledDrawables;

	bool isEnabled;
	//vector<Drawable*> children;


	float anchorXMin;
	float anchorXMax;
	float anchorYMin;
	float anchorYMax;

	//virtual void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) = 0;
	virtual void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname);

	cv::Rect screenBounds;
	void UpdateRectBounds(cv::Rect bounds) { screenBounds = bounds; };


	virtual void OnClicked();
	virtual void OnClickReleased();
};

class EmptyPanel : public Drawable
{
public:
	EmptyPanel(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
	
};


class SettingIncrementorPanel : public Drawable
{
public:
	SettingIncrementorPanel(float(*getter)(), void(*setter)(float), float increment, string label, string displayformat,
		cv::Scalar arrowcolor, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
	float(*settingGetter)();
	void(*settingSetter)(float);

};

class Label : public Drawable
{
public: 
	Label(string label, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected: 
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	string labelText;
};


class ValueLabel : public Drawable
{
public:
	ValueLabel(float(*getter)(), string label, const string displayformat, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected: 
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	float(*settingGetter)();
	string labelPrefix;
	string displayFormat;
};

class ArrowButton : public Drawable
{
public:
	//typedef float(*callback_function)(void);
	ArrowButton(float (*getter)(), void(*setter)(float), float changeamount, cv::Scalar color,
		float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);
	//ArrowButton(callback_function setter, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:

	void ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname) override;
	void OnClicked() override;
	void OnClickReleased() override;

	bool isHeld = false;
	float timeHeld = 0.0;
	const float HOLD_DELAY = 0.5; //How long you need to hold the button before you start incrementing consistently.
	const float HOLD_INCREMENT = 0.1; 

	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	float(*settingGetter)();
	void(*settingSetter)(float);
	float changeAmount;
	cv::Scalar arrowColor;
};

class ToggleButton : public Drawable
{
public: 
	ToggleButton(bool(*getter)(), void(*setter)(bool),
		float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:

	void OnClicked() override;

	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	bool(*settingGetter)();
	void(*settingSetter)(bool);
};

class ToggleButton_Record : public ToggleButton
{
public:
	ToggleButton_Record(bool(*getter)(), void(*setter)(bool),
		float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);
protected:
	//void OnClicked() override;
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
	//RecordingHelper* recordingHelper;

};

class ImageButton : public Drawable
{
public:
	ImageButton(void(*onclick)(), string imagepath, bool drawbackground,
		float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:

	void OnClicked() override;

	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	void(*onClick)();

	bool drawBackground;

	cv::Mat icon;
	cv::Mat alphamask;
};



class Sidebar; //Forward delcaring so we can use in SidebarButton. 

class SidebarButton : public Drawable
{
public:
	SidebarButton(int index, Sidebar* sidebar, string imagepath,
		float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);
	
protected:

	void OnClicked() override;

	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	int menuIndex;
	Sidebar* parentSidebar;
	//bool(Sidebar::*checkSelected)(int);
	//void(*onSelected)(int);

	cv::Mat icon;
	cv::Mat alphamask;
};



//////////Sidebar and Menus////////// 



class Sidebar : public Drawable
{
public:
	Sidebar(cv::Rect openpanelrect, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

	void ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname) override;
	void ProcessAllClicks(int x, int y, bool isdown) override;

	bool CheckSelected(int index);
	void SelectDrawable(int index);

	cv::Rect openPanelRect;

private:
	
	vector<unique_ptr<Drawable>> menus;
	int selectedIndex = 0;
	//Drawable selectedDrawable;

	//ImageButton* openSidebarButton;
	//unique_ptr<ImageButton> openSidebarButton;
	//bool isToggled = false;
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

class DisplayMenu : public Drawable
{
public:
	DisplayMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};

class EnvironmentMenu : public Drawable
{
public:
	EnvironmentMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};

class StatsMenu : public Drawable
{
public:
	StatsMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};