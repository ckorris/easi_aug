#pragma once

#include <opencv2/opencv.hpp>

using namespace std;

class Drawable
{
public:

	Drawable(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

	virtual void ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname);
	virtual void Enable();
	virtual void Disable();

protected:

	static vector<Drawable> allEnabledDrawables;

	bool isEnabled;
	vector<Drawable> children;

	float anchorXMin;
	float anchorXMax;
	float anchorYMin;
	float anchorYMax;

	virtual void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) = 0;

	cv::Rect screenBounds;
	void UpdateRectBounds(cv::Rect bounds) { screenBounds = bounds; };
};

class Clickable : public Drawable
{
public: 

	Clickable(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax) 
		: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax) {};

	static vector<Clickable*> allEnabledClickables;

	static void ProcessAllClicks(int x, int y);

	void Enable() override;

	void Disable() override;

	//void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;

protected:

	void VerifyAndExecuteClick(int xpos, int ypos);

	virtual void OnClicked() = 0;

	//virtual void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};



class EmptyPanel : public Drawable
{
public:
	EmptyPanel(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax);

private:
	void Draw(cv::Rect drawrect, cv::Mat drawto, string windowname) override;
};

class ArrowButton : public Clickable
{

};