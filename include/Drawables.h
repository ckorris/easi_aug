#pragma once

#include <opencv2/opencv.hpp>

using namespace std;

class Drawable
{
public:

	Drawable(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

	virtual void ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname);
	void Enable();
	void Disable();

	void ProcessAllClicks(int x, int y);
	vector < unique_ptr<Drawable>> children;
protected:

	const cv::Scalar TEXT_COLOR = cv::Scalar(0, 0, 255);
	const cv::Scalar BACKGROUND_COLOR = cv::Scalar(70, 70, 70);

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
	SettingIncrementorPanel(float(*getter)(), void(*setter)(float), float smallincrement, float bigincrement, 
		string label, cv::Scalar arrowcolor, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

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
	ValueLabel(float(*getter)(), string label, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected: 
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	float(*settingGetter)();
	string labelPrefix;
};

class ArrowButton : public Drawable
{
public:
	//typedef float(*callback_function)(void);
	ArrowButton(float (*getter)(), void(*setter)(float), float changeamount, cv::Scalar color,
		float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);
	//ArrowButton(callback_function setter, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:

	void OnClicked() override;

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
