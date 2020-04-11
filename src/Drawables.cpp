#include <Drawables.h>
#include <Config.h>

using namespace std;

Drawable::Drawable(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
{
	//cout << "Drawable Constructor" << endl;

	anchorXMin = anchorxmin;
	anchorXMax = anchorxmax;
	anchorYMin = anchorymin;
	anchorYMax = anchorymax;

	Enable();

}

vector<unique_ptr<Drawable>> Drawable::allEnabledDrawables;
//vector<unique_ptr<Drawable>> Drawable::children;

void Drawable::Enable()
{
	//Drawable::allEnabledDrawables.push_back(*this);

	isEnabled = true;

	//Enable children. 
	for (int i = 0; i < children.size(); i++)
	{
		//children[i].Enable();
		children[i]->Enable();
	}
}

void Drawable::Disable()
{
	isEnabled = false;

	//Disable children. 
	for (int i = 0; i < children.size(); i++)
	{
		//children[i].Disable();
		children[i]->Disable();
	}
}

void Drawable::ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname)
{
	//Only do stuff if we're enabled. 
	if (!isEnabled)
	{
		//return;
	}

	//Calculate the rect frame
	float x = parentrect.x + (parentrect.width * anchorXMin);
	float y = parentrect.y + (parentrect.height * anchorYMin);
	float width = parentrect.width * (anchorXMax - anchorXMin);
	float height = parentrect.height * (anchorYMax - anchorYMin);

	cv::Rect drawrect = cv::Rect(x, y, width, height);

	UpdateRectBounds(drawrect); //For now, just for clicking. Not in Draw() to make method easier to override. 

	//cout << "ScreenBounds: " << screenBounds.x << ", " << screenBounds.y << ", " << screenBounds.width << ", " << screenBounds.height << endl;

	Draw(drawrect, drawto, windowname);

	//Pass to children to draw. 

	//cout << "Children count: " << children.size() << endl;
	for (int i = 0; i < Drawable::children.size(); i++)
	{
		//Drawable* drawer = children[i];
		//children[i].ProcessUI(drawrect, drawto, windowname);
		Drawable::children[i]->ProcessUI(drawrect, drawto, windowname); 

		//TEST
		//children[i]->Draw(drawrect, drawto, windowname);
	}
}


void Drawable::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//cout << "Drawable Draw" << endl;
}

void Drawable::ProcessAllClicks(int x, int y)
{
	if (isEnabled == true && screenBounds.contains(cv::Point(x, y)))
	{
		OnClicked();

		for (int i = 0; i < Drawable::children.size(); i++)
		{
			//children[i].ProcessAllClicks(x, y);
			Drawable::children[i]->ProcessAllClicks(x, y);
		}
	}
	
}

void Drawable::OnClicked()
{
	
}


EmptyPanel::EmptyPanel(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax) : Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//cout << "EmptyPanel constructor" << endl;

	//Delegates for FPS speed. 
	float(*speedgetter)() = []() {return Config::forwardSpeedMPS(); };
	void(*speedsetter)(float) = [](float v) {Config::forwardSpeedMPS(v); };

	//Delegates for hop-up
	float(*hopupgetter)() = []() {return Config::hopUpSpeedMPS(); };
	void(*hopupsetter)(float) = [](float v) {Config::hopUpSpeedMPS(v); };

	//SettingIncrementorPanel speedpanel(speedgetter, speedsetter, 0, 0.15, 0, 1);

	//Drawable::children.emplace_back(new ArrowButton(speedgetter, speedsetter, 1.0, 0, 0.8, 0, .2));
	Drawable::children.emplace_back(new SettingIncrementorPanel(speedgetter, speedsetter, "FPS: ", 0, 0.1, 0, 1));
	Drawable::children.emplace_back(new SettingIncrementorPanel(hopupgetter, hopupsetter, "HOP: ", 0.1, 0.2, 0, 1));
	
}

void EmptyPanel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//cout << "EmptyPanel draw" << endl;
	cv::rectangle(drawto, drawrect, cv::Scalar(25, 25, 25), 2); //Draws an outline around its bounds. 
}

SettingIncrementorPanel::SettingIncrementorPanel(float(*getter)(), void(*setter)(float), string label,
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	:Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	settingGetter = getter;
	settingSetter = setter;

	//ArrowButton plusfivearrow(getter, setter, 5.0, 0, 1, 0, 0.17);
	//ArrowButton plusonearrow(getter, setter, 1.0, 0, 1, 0.17, 0.34);

	Drawable::children.emplace_back(new ArrowButton(getter, setter, 5.0, 0, 1, 0.0, 0.2)); //Plus 5 arrow.
	Drawable::children.emplace_back(new ArrowButton(getter, setter, 1.0, 0, 1, 0.2, 0.4)); // Plus 1 arrow.

	Drawable::children.emplace_back(new ValueLabel(getter, label, 0, 1, 0.4, 0.6)); // Plus 1 arrow.

	Drawable::children.emplace_back(new ArrowButton(getter, setter, -1.0, 0, 1, .6, 0.8)); //Minus 1 arrow.
	Drawable::children.emplace_back(new ArrowButton(getter, setter, -5.0, 0, 1, 0.8, 1)); //Minus 5 arrow.
}

void SettingIncrementorPanel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Don't do anything. 
}

ValueLabel::ValueLabel(float(*getter)(), string label, 
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	settingGetter = getter;
	labelPrefix = label;
}

void ValueLabel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	char buffer[20];
	int n = sprintf(buffer, "%1.1f", this->settingGetter());


	string printval = labelPrefix + buffer;

	cv::Size textsize = cv::getTextSize(printval, 1, 1, 1, NULL);

	cv::Point centerpoint(screenBounds.x + (screenBounds.width / 2.0) - (textsize.width / 2.0), 
		screenBounds.y + (screenBounds.height / 2.0) + (textsize.height / 2.0));
	cv::putText(drawto, printval, centerpoint, 1, 1, cv::Scalar(255, 255, 255), 1, 8, false);
}

ArrowButton::ArrowButton(float (*getter)(), void(*setter)(float), float changeamount, 
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//cout << "ArrowButton constructor" << endl;

	settingGetter = getter;
	settingSetter = setter;
	changeAmount = changeamount;
}


void ArrowButton::OnClicked()
{

	float val = this->settingGetter();
	val += changeAmount;
	this->settingSetter(val);
	cout << "New setting: " << val << endl;
}

void ArrowButton::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cv::rectangle(drawto, screenBounds, cv::Scalar(0, 0, 255), -1); //Background. 

	vector<cv::Point> points;
	if (changeAmount >= 0.0)
	{
		points.push_back(cv::Point(screenBounds.x, screenBounds.y + screenBounds.height));
		points.push_back(cv::Point(screenBounds.x + (screenBounds.width / 2.0), screenBounds.y));
		points.push_back(cv::Point(screenBounds.x + screenBounds.width, screenBounds.y + screenBounds.height));
	}
	else
	{
		points.push_back(cv::Point(screenBounds.x, screenBounds.y));
		points.push_back(cv::Point(screenBounds.x + (screenBounds.width / 2.0), screenBounds.y + screenBounds.height));
		points.push_back(cv::Point(screenBounds.x + screenBounds.width, screenBounds.y));
	}

	cv::fillConvexPoly(drawto, points, cv::Scalar(140, 140, 140));
}