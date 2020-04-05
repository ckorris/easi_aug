#include <Drawables.h>
#include <Config.h>

using namespace std;

Drawable::Drawable(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
{
	cout << "Drawable Constructor" << endl;

	anchorXMin = anchorxmin;
	anchorXMax = anchorxmax;
	anchorYMin = anchorymin;
	anchorYMax = anchorymax;

	Enable();

}

void Drawable::Enable()
{
	//Drawable::allEnabledDrawables.push_back(*this);

	isEnabled = true;

	//Enable children. 
	for (int i = 0; i < children.size(); i++)
	{
		children[i].Enable();
	}
}

void Drawable::Disable()
{
	isEnabled = false;

	//Disable children. 
	for (int i = 0; i < children.size(); i++)
	{
		children[i].Disable();
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
	for (int i = 0; i < children.size(); i++)
	{
		children[i].ProcessUI(drawrect, drawto, windowname);

		//TEST
		children[i].Draw(drawrect, drawto, windowname);
	}
}


void Drawable::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cout << "Drawable Draw" << endl;
}

void Drawable::ProcessAllClicks(int x, int y)
{
	
	
	if (isEnabled == true && screenBounds.contains(cv::Point(x, y)))
	{
		OnClicked();

		for (int i = 0; i < children.size(); i++)
		{
			children[i].ProcessAllClicks(x, y);
		}
	}
	
}

void Drawable::OnClicked()
{
	//cout << "Drawable base draw" << endl;
}

float EmptyPanel::GetSpeed()
{
	//return 6.5;
	cout << "Retrieved forward speed!" << endl;
	return Config::forwardSpeedMPS();
}



EmptyPanel::EmptyPanel(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax) : Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//cout << "EmptyPanel constructor" << endl;

	//float getspeed() { return Config::forwardSpeedMPS(); };
	//ArrowButton::callback_function speeddel;
	float(*del)() = GetSpeed; //TODO: Lambda?
	//float (*del)() = Config::forwardSpeedMPS;
	//speeddel = &EmptyPanel::GetSpeed();
	ArrowButton uparrow(del, 0, 0.8, 0, .2);

	children.push_back(uparrow);
	
}



void EmptyPanel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cout << "EmptyPanel draw" << endl;
	cv::rectangle(drawto, drawrect, cv::Scalar(25, 25, 25), 2); //Test - should be empty eventually. 
}


ArrowButton::ArrowButton(float (*setter)(), float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	cout << "ArrowButton constructor" << endl;

	settingSetter = setter;
}


void ArrowButton::OnClicked()
{
	this->settingSetter();
}

void ArrowButton::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cout << "ArrowButton Draw" << endl;
	//For now just draw rectangle.
	//cout << "ScreenBounds: " << screenBounds.x << ", " << screenBounds.y << ", " << screenBounds.width << ", " << screenBounds.height << endl;
	//cv::rectangle(drawto, screenBounds, cv::Scalar(0, 0, 255), 5);
	cv::rectangle(drawto, cv::Rect(200, 200, 200, 200), cv::Scalar(0, 0, 255), 5);
}