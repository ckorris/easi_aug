#include <Drawables.h>

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
	/*vector<Drawable>::iterator findresults = find(Drawable::allEnabledDrawables.begin(), Drawable::allEnabledDrawables.end(), this);
	if (findresults != Drawable::allEnabledDrawables.end)
	{
		Drawable::allEnabledDrawables.erase(findresults);
	}
	else
	{
		cout << "Tried to disable Drawable, but it did not exist in the allEnabledDrawables vector." << endl;
	}*/

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
		return;
	}

	//Calculate the rect frame
	float x = parentrect.x + (parentrect.width * anchorXMin);
	float y = parentrect.y + (parentrect.height * anchorYMin);
	float width = parentrect.width * (anchorXMax - anchorXMin);
	float height = parentrect.height * (anchorYMax - anchorYMin);

	cv::Rect drawrect = cv::Rect(x, y, width, height);

	UpdateRectBounds(drawrect); //For now, just for clicking. Not in Draw() to make method easier to override. 

	Draw(drawrect, drawto, windowname);

	//Pass to children to draw. 

	for (int i = 0; i < children.size(); i++)
	{
		children[i].ProcessUI(drawrect, drawto, windowname);
	}
}


/*
void Clickable::ProcessAllClicks(int x, int y)
{
	for (int i = 0; i < allEnabledClickables.size(); i++)
	{
		allEnabledClickables[i].VerifyAndExecuteClick(x, y);
	}
}

void Clickable::Enable()
{
	Drawable::Enable();
	allEnabledClickables.push_back(*this);
}*/

void Clickable::Disable()
{
	Drawable::Disable();

	//vector<Clickable>::iterator findresults = find(Clickable::allEnabledClickables.begin(), Clickable::allEnabledClickables.end(), this);
	vector<Clickable*>::iterator findresults = find(Clickable::allEnabledClickables.begin(), Clickable::allEnabledClickables.end(), this);
	//Clickable::allEnabledClickables.erase(findresults);
	/*if (findresults != Clickable::allEnabledClickables.end())
	{
		Clickable::allEnabledClickables.erase(findresults);
	}
	else
	{
		cout << "Tried to disable Drawable, but it did not exist in the allEnabledDrawables vector." << endl;
	}*/

}

void Clickable::VerifyAndExecuteClick(int xpos, int ypos)
{
	if (isEnabled == true && screenBounds.contains(cv::Point(xpos, ypos)))
	{
		//OnClicked();
	}
}


/*
void Clickable::ProcessClick(cv::Rect parentrect, int xpos, int ypos)
{
	//TODO: We calculate the rect in both Drawable.ProcessUI and this. Just do once per frame. 

	//Only do stuff if we're enabled. 
	if (!isEnabled)
	{
		return;
	}

	//Calculate the rect frame
	float x = parentrect.x + (parentrect.width * anchorXMin);
	float y = parentrect.y + (parentrect.height * anchorYMin);
	float width = parentrect.width * (anchorXMax - anchorXMin);
	float height = parentrect.height * (anchorYMax - anchorYMin);

	cv::Rect clickrect = cv::Rect(x, y, width, height);

	
	//Pass to children to process click. 
	for (int i = 0; i < children.size(); i++)
	{
		Clickable* clickable = dynamic_cast<Clickable*>(&children[i]);

		if (clickable)
		{
			clickable->ProcessClick(parentrect, xpos, ypos);
		}
	}
}
*/

EmptyPanel::EmptyPanel(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax) : Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	cout << "EmptyPanel constructor" << endl;
}

void EmptyPanel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cv::rectangle(drawto, drawrect, cv::Scalar(25, 25, 25), 2); //Test - should be empty eventually. 
}