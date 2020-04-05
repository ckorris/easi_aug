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

protected:

	//static vector<Drawable> allEnabledDrawables;

	bool isEnabled;
	vector<Drawable> children;

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
	
	static float GetSpeed();
};


class ArrowButton : public Drawable
{
public:
	typedef float(*callback_function)(void);
	ArrowButton(float (*setter)(), float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);
	//ArrowButton(callback_function setter, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

protected:

	void OnClicked() override;

	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

	float(*settingSetter)();
};
