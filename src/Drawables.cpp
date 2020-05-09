#include <Drawables.h>
#include <Config.h>
#include <TimeHelper.h>
//#include <Menus.h>

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

void Drawable::ProcessAllClicks(int x, int y, bool isdown)
{
	if (isEnabled == true && screenBounds.contains(cv::Point(x, y)))
	{
		if (isdown == true)
		{
			OnClicked();
		}
		else
		{
			OnClickReleased();
		}

		for (int i = 0; i < Drawable::children.size(); i++)
		{
			//children[i].ProcessAllClicks(x, y);
			Drawable::children[i]->ProcessAllClicks(x, y, isdown);
		}
	}
	
}

void Drawable::OnClicked()
{
	
}

void Drawable::OnClickReleased()
{

}


EmptyPanel::EmptyPanel(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax) : Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//cout << "EmptyPanel constructor" << endl;

	//Speed arrows.
	float(*speedgetter)() = []() {return Config::forwardSpeedMPS(); };
	void(*speedsetter)(float) = [](float v) {Config::forwardSpeedMPS(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(speedgetter, speedsetter, 1, "FPS ", "%1.1f", cv::Scalar(0, 0, 255, 1), 0, 0.1, 0, 1));
	
	//Hop-up arrows.
	float(*hopupgetter)() = []() {return Config::hopUpRPM(); };
	void(*hopupsetter)(float) = [](float v) {Config::hopUpRPM(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(hopupgetter, hopupsetter, 1000, "HOP ","%1.0f", cv::Scalar(0, 0, 255, 1), 0.1, 0.2, 0, 1));

	//Draw toggles. 
	//Toggle laser crosshair.
	bool(*drawLaserCrosshairGetter)() = []() { return Config::toggleLaserCrosshair(); };
	void(*drawLaserCrosshairSetter)(bool) = [](bool b) { Config::toggleLaserCrosshair(b); };
	Drawable::children.emplace_back(new Label("L-XHR:", 0.2, 0.25, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawLaserCrosshairGetter, drawLaserCrosshairSetter, 0.25, 0.3, 0, 0.5));
	//Toggle gravity crosshair.
	bool(*drawGravityCrosshairGetter)() = []() { return Config::toggleGravityCrosshair(); };
	void(*drawGravityCrosshairSetter)(bool) = [](bool b) { Config::toggleGravityCrosshair(b); };
	Drawable::children.emplace_back(new Label("G-XHR:", 0.2, 0.25, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(drawGravityCrosshairGetter, drawGravityCrosshairSetter, 0.25, 0.3, 0.5, 1));
	//Toggle laser path.
	bool(*drawLaserPathGetter)() = []() { return Config::toggleLaserPath(); };
	void(*drawLaserPathSetter)(bool) = [](bool b) { Config::toggleLaserPath(b); };
	Drawable::children.emplace_back(new Label("L-PTH:", 0.3, 0.35, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawLaserPathGetter, drawLaserPathSetter, 0.35, 0.4, 0, 0.5));
	//Toggle gravity path.q
	bool(*drawGravityPathGetter)() = []() { return Config::toggleGravityPath(); };
	void(*drawGravityPathSetter)(bool) = [](bool b) { Config::toggleGravityPath(b); };
	Drawable::children.emplace_back(new Label("G-PTH:", 0.3, 0.35, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(drawGravityPathGetter, drawGravityPathSetter, 0.35, 0.4, 0.5, 1));
	//Toggle distance.
	bool(*drawDistanceGetter)() = []() { return Config::toggleDistance(); };
	void(*drawDistanceSetter)(bool) = [](bool b) { Config::toggleDistance(b); };
	Drawable::children.emplace_back(new Label("DIST:", 0.4, 0.45, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawDistanceGetter, drawDistanceSetter, 0.45, 0.5, 0, 0.5));
	//Toggle travel time.
	bool(*travelTimeGetter)() = []() { return Config::toggleTravelTime(); };
	void(*travelTimeSetter)(bool) = [](bool b) { Config::toggleTravelTime(b); };
	Drawable::children.emplace_back(new Label("TIME:", 0.4, 0.45, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(travelTimeGetter, travelTimeSetter, 0.45, 0.5, 0.5, 1));

	//Cam pos arrows.
	//X pos.
	float(*camXPosGetter)() = []() {return Config::camXPos(); };
	void(*camXPosSetter)(float) = [](float v) {Config::camXPos(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camXPosGetter, camXPosSetter, 0.01, "X-POS ", "%1.3f", cv::Scalar(0, 255, 0, 1), 0.52, 0.59, 0, 1));
	//Y pos.
	float(*camYPosGetter)() = []() {return Config::camYPos(); };
	void(*camYPosSetter)(float) = [](float v) {Config::camYPos(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camYPosGetter, camYPosSetter, 0.01, "Y-POS ", "%1.3f", cv::Scalar(0, 0, 255, 1), 0.60, 0.67, 0, 1));
	//Z pos.
	float(*camZPosGetter)() = []() {return Config::camZPos(); };
	void(*camZPosSetter)(float) = [](float v) {Config::camZPos(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camZPosGetter, camZPosSetter, 0.01, "Z-POS ", "%1.3f", cv::Scalar(255, 0, 0, 1), 0.68, 0.75, 0, 1));

	//Cam rot arrows.
	//X rot. 
	float(*camXRotGetter)() = []() {return Config::camXRot(); };
	void(*camXRotSetter)(float) = [](float v) {Config::camXRot(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camXRotGetter, camXRotSetter, 0.05, "X-ROT: ", "%1.3f", cv::Scalar(0, 255, 0, 1), 0.77, 0.84, 0, 1));
	//Y rot. 
	float(*camYRotGetter)() = []() {return Config::camYRot(); };
	void(*camYRotSetter)(float) = [](float v) {Config::camYRot(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camYRotGetter, camYRotSetter, 0.05, "Y-ROT ", "%1.3f", cv::Scalar(0, 0, 255, 1), 0.85, 0.92, 0, 1));
	//Z rot. 
	float(*camZRotGetter)() = []() {return Config::camZRot(); };
	void(*camZRotSetter)(float) = [](float v) {Config::camZRot(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camZRotGetter, camZRotSetter, 0.05, "Z-ROT ", "%1.3f", cv::Scalar(255, 0, 0, 1), 0.93, 1, 0, 1));
}

void EmptyPanel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//cout << "EmptyPanel draw" << endl;
	//Draws an outline around its bounds. 
	cv::rectangle(drawto, drawrect, cv::Scalar(25, 25, 25, 1), 2); 
	//Draw a gray background. 
	cv::rectangle(drawto, drawrect, BACKGROUND_COLOR, -1);
}

SettingIncrementorPanel::SettingIncrementorPanel(float(*getter)(), void(*setter)(float), float increment, string label,
	string displayformat, cv::Scalar arrowcolor, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	settingGetter = getter;
	settingSetter = setter;


	//ArrowButton plusfivearrow(getter, setter, 5.0, 0, 1, 0, 0.17);
	//ArrowButton plusonearrow(getter, setter, 1.0, 0, 1, 0.17, 0.34);

	Drawable::children.emplace_back(new Label(label, 0, 1, 0, 0.167));
	//Drawable::children.emplace_back(new ArrowButton(getter, setter, bigincrement, arrowcolor, 0, 1, 0.0, 0.2)); //Plus 5 arrow.
	//Drawable::children.emplace_back(new ArrowButton(getter, setter, smallincrement, arrowcolor, 0, 1, 0.2, 0.4)); // Plus 1 arrow.
	Drawable::children.emplace_back(new ArrowButton(getter, setter, increment, arrowcolor, 0, 1, 0.167, 0.5)); // Plus 1 arrow.

	Drawable::children.emplace_back(new ValueLabel(getter, "", displayformat, 0, 1, 0.5, 0.667)); // Plus 1 arrow.

	//Drawable::children.emplace_back(new ArrowButton(getter, setter, -smallincrement, arrowcolor, 0, 1, .6, 0.8)); //Minus 1 arrow.
	//Drawable::children.emplace_back(new ArrowButton(getter, setter, -bigincrement, arrowcolor, 0, 1, 0.8, 1)); //Minus 5 arrow.
	Drawable::children.emplace_back(new ArrowButton(getter, setter, -increment, arrowcolor, 0, 1, 0.667, 1)); //Minus 5 arrow.
}

void SettingIncrementorPanel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Don't do anything. 
}

Label::Label(string label, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	labelText = label;
}

void Label::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cv::Size textsize = cv::getTextSize(labelText, 1, 1, 1, NULL);

	cv::Point centerpoint(screenBounds.x + (screenBounds.width / 2.0) - (textsize.width / 2.0),
		screenBounds.y + (screenBounds.height / 2.0) + (textsize.height / 2.0));
	//cv::putText(drawto, labelText, centerpoint, 1, 1, cv::Scalar(255, 255, 255, 1), 1, 8, false);
	cv::putText(drawto, labelText, centerpoint, 1, 1, TEXT_COLOR, 1, 8, false);
}



ValueLabel::ValueLabel(float(*getter)(), string label, string displayformat,
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	settingGetter = getter;
	labelPrefix = label;
	displayFormat = displayformat;
}

void ValueLabel::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	char buffer[20];
	//int n = sprintf(buffer, "%1.2f", this->settingGetter());
	int n = sprintf(buffer, displayFormat.c_str(), this->settingGetter());

	string printval = labelPrefix + buffer;

	cv::Size textsize = cv::getTextSize(printval, 1, 1, 1, NULL);

	cv::Point centerpoint(screenBounds.x + (screenBounds.width / 2.0) - (textsize.width / 2.0), 
		screenBounds.y + (screenBounds.height / 2.0) + (textsize.height / 2.0));
	//cv::putText(drawto, printval, centerpoint, 1, 1, cv::Scalar(255, 255, 255), 1, 8, false);
	cv::putText(drawto, printval, centerpoint, 1, 1, TEXT_COLOR, 1, 8, false);
}

ArrowButton::ArrowButton(float (*getter)(), void(*setter)(float), float changeamount, cv::Scalar color,
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//cout << "ArrowButton constructor" << endl;

	settingGetter = getter;
	settingSetter = setter;
	changeAmount = changeamount;
	arrowColor = color;
}

void ArrowButton::ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname)
{
	Drawable::ProcessUI(parentrect, drawto, windowname);

	if (isHeld == true)
	{
		timeHeld += Time::deltaTime();
		if (timeHeld - HOLD_DELAY >= HOLD_INCREMENT)
		{
			timeHeld = HOLD_DELAY + remainder(timeHeld - HOLD_DELAY, HOLD_INCREMENT);
			OnClicked();
		}
	}
}


void ArrowButton::OnClicked()
{
	if (isHeld == false)
	{
		isHeld = true;
		timeHeld = 0.0;
	}

	float val = this->settingGetter();
	val += changeAmount;
	this->settingSetter(val);
	//cout << "New setting: " << val << endl;
}

void ArrowButton::OnClickReleased()
{
	isHeld = false;
}

void ArrowButton::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	cv::rectangle(drawto, screenBounds, cv::Scalar(0, 0, 0, 1), -1); //Black background. 

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

	//cv::fillConvexPoly(drawto, points, cv::Scalar(0, 0, 255, 1));
	cv::fillConvexPoly(drawto, points, arrowColor);
}

ToggleButton::ToggleButton(bool(*getter)(), void(*setter)(bool),
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	settingGetter = getter;
	settingSetter = setter;
}

void ToggleButton::OnClicked()
{
	//Flip the assigned bool setting. 
	settingSetter(!settingGetter());
}

void ToggleButton::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Black outline. 
	cv::rectangle(drawto, screenBounds, cv::Scalar(0, 0, 0, 255), 2);
	//Dark gray fill.
	cv::rectangle(drawto, screenBounds, cv::Scalar(30, 30, 30, 255), -1);

	//If enabled, draw X. 
	if (settingGetter())
	{
		const float marginpercent = 0.15; //Percentage of the bounds reserved for margin. 
		float marginwidth = screenBounds.width * marginpercent;
		float marginheight = screenBounds.height * marginpercent;

		//Line from top left to bottom right.
		cv::line(drawto, cv::Point(screenBounds.x + marginwidth, screenBounds.y + marginwidth),
			cv::Point(screenBounds.x + screenBounds.width - marginwidth, screenBounds.y + screenBounds.height - marginwidth),
			cv::Scalar(255, 255, 255, 1), 3);
		//Line from bottom left to top right.
		cv::line(drawto, cv::Point(screenBounds.x + marginwidth, screenBounds.y + screenBounds.height - marginwidth),
			cv::Point(screenBounds.x + screenBounds.width - marginwidth, screenBounds.y + marginwidth),
			cv::Scalar(255, 255, 255, 1), 3);
	}
}

ImageButton::ImageButton(void(*onclick)(), string imagepath, cv::Scalar backgroundcolor, 
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	onClick = onclick;
	backgroundColor = backgroundcolor;

	cv::Mat rawicon = cv::imread(imagepath, -1);
	if (rawicon.empty())
	{
		cout << "Couldn't find image." << endl;
	}

	icon = cv::Mat(rawicon.rows, rawicon.cols, CV_8UC4);
	icon.setTo(cv::Scalar(0, 0, 0, 255));
	int fromto[]{ 0, 0, 1, 1, 2, 2 };
	cv::mixChannels(rawicon, icon, fromto, 3);

	//Alpha mask. 
	alphamask = cv::Mat(icon.rows, icon.cols, CV_8UC1);
	int alphafromto[]{ 3, 0 };
	cv::mixChannels(rawicon, alphamask, alphafromto, 1);

	rawicon.release();
}

void ImageButton::OnClicked()
{
	onClick();
}

void ImageButton::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Draw background rectangle. 
	cv::rectangle(drawto, screenBounds, backgroundColor, -1);

	//Draw outline rectangle.
	cv::rectangle(drawto, screenBounds, cv::Scalar(0, 0, 0, 255), 1);

	//Draw image on button. 
	cv::Rect iconrect = cv::Rect(screenBounds.x + 4, screenBounds.y + 4, screenBounds.width - 8, screenBounds.height - 8);
	if (icon.cols != iconrect.width || icon.rows != iconrect.height)
	{
		cv::Size newsize(screenBounds.width - 8, screenBounds.height - 8);
		cv::resize(icon, icon, newsize);
		cv::resize(alphamask, alphamask, newsize);
	}
	//cv::Rect iconrect = cv::Rect(screenBounds.x + 4, screenBounds.y + 4, screenBounds.width - 8, screenBounds.height - 8);
	cv::Mat imageroi = drawto(iconrect);
	cv::bitwise_and(icon, icon, imageroi, alphamask);
}


//TODO: Inherit from ImageButton? 
SidebarButton::SidebarButton(int index, Sidebar* sidebar, string imagepath,
	float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	menuIndex = index;
	parentSidebar = sidebar;

	cv::Mat rawicon = cv::imread(imagepath, -1);
	if (rawicon.empty())
	{
		cout << "Couldn't find image." << endl;
	}

	icon = cv::Mat(rawicon.rows, rawicon.cols, CV_8UC4);
	icon.setTo(cv::Scalar(0, 0, 0, 255));
	int fromto[]{ 0, 0, 1, 1, 2, 2};
	cv::mixChannels(rawicon, icon, fromto, 3);

	//Alpha mask. 
	alphamask = cv::Mat(icon.rows, icon.cols, CV_8UC1);
	int alphafromto[]{ 3, 0 };
	cv::mixChannels(rawicon, alphamask, alphafromto, 1);
	
	rawicon.release();
}

void SidebarButton::OnClicked()
{
	//onSelected(menuIndex);
	parentSidebar->SelectDrawable(menuIndex);
}

void SidebarButton::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	bool selected = parentSidebar->CheckSelected(menuIndex);
	//bool selected = checkSelected(menuIndex);
	//bool selected = true;
	if (!selected) 
	{
		//Draw a light rectangle with lighter shading on top and left to show it sticking out. 
		cv::rectangle(drawto, screenBounds, cv::Scalar(200, 200, 200, 255), -1);
		cv::line(drawto, cv::Point(screenBounds.x, screenBounds.y), cv::Point(screenBounds.x + screenBounds.width, screenBounds.y),
			cv::Scalar(220, 220, 220, 255), 1);
		cv::line(drawto, cv::Point(screenBounds.x, screenBounds.y), cv::Point(screenBounds.x, screenBounds.y + screenBounds.height),
			cv::Scalar(210, 210, 210, 255), 1);
	}
	else
	{
		//Draw a dark rectangle with darker shading on bottom and right to show it sticking in. 
		cv::rectangle(drawto, screenBounds, cv::Scalar(120, 120, 120, 255), -1);
		cv::line(drawto, cv::Point(screenBounds.x, screenBounds.y + screenBounds.height), 
			cv::Point(screenBounds.x + screenBounds.width, screenBounds.y + screenBounds.height),
			cv::Scalar(80, 80, 80, 255), 1);
		cv::line(drawto, cv::Point(screenBounds.x + screenBounds.width, screenBounds.y), 
			cv::Point(screenBounds.x + screenBounds.width, screenBounds.y + screenBounds.height),
			cv::Scalar(120, 120, 120, 255), 1);
	}

	//Draw image on button. 
	cv::Rect iconrect = cv::Rect(screenBounds.x + 4, screenBounds.y + 4, screenBounds.width - 8, screenBounds.height - 8);
	if (icon.cols != iconrect.width || icon.rows != iconrect.height)
	{
		cv::Size newsize(screenBounds.width - 8, screenBounds.height - 8);
		cv::resize(icon, icon, newsize);
		cv::resize(alphamask, alphamask, newsize);
	}
	//cv::Rect iconrect = cv::Rect(screenBounds.x + 4, screenBounds.y + 4, screenBounds.width - 8, screenBounds.height - 8);
	cv::Mat imageroi = drawto(iconrect);
	cv::bitwise_and(icon, icon, imageroi, alphamask);

}


