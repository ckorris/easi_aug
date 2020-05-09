//#include <Menus.h>
#include <Drawables.h>
#include <Config.h>




Sidebar::Sidebar(cv::Rect openpanelrect, float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	openPanelRect = openpanelrect;
	//calibMenu = CalibrationMenu(0, 1, 0, 1);
	//projMenu = ProjectileMenu(0, 1, 0, 1);

	//Calibration menu.
	menus.emplace_back(new CalibrationMenu(0, 1, 0, 1));
	menus.emplace_back(new ProjectileMenu(0, 1, 0, 1));
	menus.emplace_back(new DisplayMenu(0, 1, 0, 1));
	menus.emplace_back(new EnvironmentMenu(0, 1, 0, 1));
	menus.emplace_back(new StatsMenu(0, 1, 0, 1));


	//Buttons. For now, no images, just indexes.
	//bool(*checkselected)(int) = [this](int i) { return i == selectedIndex; };
	//auto checkselected = [this](int i) { return i == selectedIndex; };
	//bool (*checkselected)(int index);
	//checkselected = this->CheckSelected;

	//bool(*checkselected)() = []() { return true; };
	bool(Sidebar::*checkselected)(int index);
	checkselected = &Sidebar::CheckSelected;

	//auto setselected = [this](int i) { SelectDrawable(i); };
	//void(*setselected)(int index);
	//setselected = this->SelectDrawable;

	//Calibration.
	Drawable::children.emplace_back(new SidebarButton(0, this, "../images/calib_icon.png", 0.0, 1.0, 0.0, 0.2));
	//Projectile.
	Drawable::children.emplace_back(new SidebarButton(1, this, "../images/projectile_icon.png", 0.0, 1.0, 0.2, 0.4));
	//Display.
	Drawable::children.emplace_back(new SidebarButton(2, this, "../images/display_icon.png", 0.0, 1.0, 0.4, 0.6));
	//Environment.
	Drawable::children.emplace_back(new SidebarButton(3, this, "../images/environment_icon.png", 0.0, 1.0, 0.6, 0.8));
	//Stats.
	Drawable::children.emplace_back(new SidebarButton(4, this, "../images/stats_icon.png", 0.0, 1.0, 0.8, 1.0));

}

void Sidebar::ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname)
{
	Drawable::ProcessUI(parentrect, drawto, windowname); //Base. 
	
	//Process the UI for only the enabled menu.
	if (selectedIndex >= 0 && selectedIndex < menus.size())
	{ 
		menus[selectedIndex]->ProcessUI(openPanelRect, drawto, windowname);
	}
	
}

void Sidebar::ProcessAllClicks(int x, int y)
{
	Drawable::ProcessAllClicks(x, y); //Base.

	//Process the clicks for only the enabled menu. 
	if (selectedIndex >= 0 && selectedIndex < menus.size())
	{
		menus[selectedIndex]->ProcessAllClicks(x, y);
	}

}

bool Sidebar::ReturnTrue()
{
	return true;
}

bool Sidebar::CheckSelected(int index)
{
	return index == selectedIndex;
}

void Sidebar::SelectDrawable(int index)
{
	//TODO: More. 
	selectedIndex = index;
}






CalibrationMenu::CalibrationMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//Cam pos arrows.
	//X pos.
	float(*camXPosGetter)() = []() {return Config::camXPos(); };
	void(*camXPosSetter)(float) = [](float v) {Config::camXPos(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camXPosGetter, camXPosSetter, 0.01, "X-POS", "%1.3fm", cv::Scalar(0, 255, 0, 1), 0.01, 0.16, 0, 1));
	//Y pos.
	float(*camYPosGetter)() = []() {return Config::camYPos(); };
	void(*camYPosSetter)(float) = [](float v) {Config::camYPos(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camYPosGetter, camYPosSetter, 0.01,  "Y-POS", "%1.3fm", cv::Scalar(0, 0, 255, 1), 0.17, 0.32, 0, 1));
	//Z pos.
	float(*camZPosGetter)() = []() {return Config::camZPos(); };
	void(*camZPosSetter)(float) = [](float v) {Config::camZPos(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camZPosGetter, camZPosSetter, 0.01, "Z-POS", "%1.3fm", cv::Scalar(255, 0, 0, 1), 0.33, 0.48, 0, 1));

	//Cam rot arrows.
	//X rot. 
	float(*camXRotGetter)() = []() {return Config::camXRot(); };
	void(*camXRotSetter)(float) = [](float v) {Config::camXRot(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camXRotGetter, camXRotSetter, 0.1, "X-ROT", "%1.3f", cv::Scalar(0, 255, 0, 1), 0.53, 0.68, 0, 1));
	//Y rot. 
	float(*camYRotGetter)() = []() {return Config::camYRot(); };
	void(*camYRotSetter)(float) = [](float v) {Config::camYRot(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camYRotGetter, camYRotSetter, 0.1, "Y-ROT", "%1.3f", cv::Scalar(0, 0, 255, 1), 0.69, 0.84, 0, 1));
	//Z rot. 
	float(*camZRotGetter)() = []() {return Config::camZRot(); };
	void(*camZRotSetter)(float) = [](float v) {Config::camZRot(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(camZRotGetter, camZRotSetter, 0.1, "Z-ROT", "%1.3f", cv::Scalar(255, 0, 0, 1), 0.85, 1, 0, 1));
}

void CalibrationMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Don't do anything special. Should draw all of the above automatically. 
}



ProjectileMenu::ProjectileMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//Speed arrows.
	float(*speedgetter)() = []() {return Config::forwardSpeedMPS(); };
	void(*speedsetter)(float) = [](float v) {Config::forwardSpeedMPS(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(speedgetter, speedsetter, 1, "SPEED", "%1.1f m/s", cv::Scalar(0, 0, 255, 1), 0.01, 0.31, 0, 1));

	//Hop-up arrows.
	float(*hopupgetter)() = []() {return Config::hopUpRPM(); };
	void(*hopupsetter)(float) = [](float v) {Config::hopUpRPM(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(hopupgetter, hopupsetter, 1000, "HOP-UP", "%1.0f RPM", cv::Scalar(0, 0, 255, 1), 0.35, 0.65, 0, 1));

	//Mass arrows.
	float(*massgetter)() = []() {return Config::bbMassGrams(); };
	void(*masssetter)(float) = [](float v) {Config::bbMassGrams(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(massgetter, masssetter, 0.01, "BB MASS", "%1.2fg", cv::Scalar(0, 0, 255, 1), 0.69, 0.99, 0, 1));

}

void ProjectileMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Don't do anything special. Should draw all of the above automatically. 
}


DisplayMenu::DisplayMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//Draw toggles. 
	//Toggle laser crosshair.
	bool(*drawLaserCrosshairGetter)() = []() { return Config::toggleLaserCrosshair(); };
	void(*drawLaserCrosshairSetter)(bool) = [](bool b) { Config::toggleLaserCrosshair(b); };
	Drawable::children.emplace_back(new Label("L-XHR:", 0.0, 0.125, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawLaserCrosshairGetter, drawLaserCrosshairSetter, 0.125, 0.25, 0, 0.5));
	//Toggle gravity crosshair.
	bool(*drawGravityCrosshairGetter)() = []() { return Config::toggleGravityCrosshair(); };
	void(*drawGravityCrosshairSetter)(bool) = [](bool b) { Config::toggleGravityCrosshair(b); };
	Drawable::children.emplace_back(new Label("G-XHR:", 0.0, 0.125, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(drawGravityCrosshairGetter, drawGravityCrosshairSetter, 0.125, 0.25, 0.5, 1));
	//Toggle laser path.
	bool(*drawLaserPathGetter)() = []() { return Config::toggleLaserPath(); };
	void(*drawLaserPathSetter)(bool) = [](bool b) { Config::toggleLaserPath(b); };
	Drawable::children.emplace_back(new Label("L-PTH:", 0.25, 0.375, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawLaserPathGetter, drawLaserPathSetter, 0.375, 0.5, 0, 0.5));
	//Toggle gravity path.q
	bool(*drawGravityPathGetter)() = []() { return Config::toggleGravityPath(); };
	void(*drawGravityPathSetter)(bool) = [](bool b) { Config::toggleGravityPath(b); };
	Drawable::children.emplace_back(new Label("G-PTH:", 0.25, 0.375, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(drawGravityPathGetter, drawGravityPathSetter, 0.375, 0.5, 0.5, 1));
	//Toggle distance.
	bool(*drawDistanceGetter)() = []() { return Config::toggleDistance(); };
	void(*drawDistanceSetter)(bool) = [](bool b) { Config::toggleDistance(b); };
	Drawable::children.emplace_back(new Label("DIST:", 0.5, 0.625, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawDistanceGetter, drawDistanceSetter, 0.625, 0.75, 0, 0.5));
	//Toggle travel time.
	bool(*travelTimeGetter)() = []() { return Config::toggleTravelTime(); };
	void(*travelTimeSetter)(bool) = [](bool b) { Config::toggleTravelTime(b); };
	Drawable::children.emplace_back(new Label("TIME:", 0.5, 0.625, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(travelTimeGetter, travelTimeSetter, 0.625, 0.75, 0.5, 1));
}

void DisplayMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//Don't do anything special. Should draw all of the above automatically. 
}

EnvironmentMenu::EnvironmentMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//Temperature arrows.
	float(*tempgetter)() = []() {return Config::temperatureC(); };
	void(*tempsetter)(float) = [](float v) {Config::temperatureC(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(tempgetter, tempsetter, 1, "TEMP", "%1.0f C", cv::Scalar(0, 0, 255, 1), 0.01, 0.31, 0, 1));

	//Relative humidity arrows.
	float(*relhumidgetter)() = []() {return Config::relativeHumidity01(); };
	void(*relhumidsetter)(float) = [](float v) {Config::relativeHumidity01(v); };
	Drawable::children.emplace_back(new SettingIncrementorPanel(relhumidgetter, relhumidsetter, 1000, "REL. HUMID", "%1.0f%%", cv::Scalar(0, 0, 255, 1), 0.35, 0.65, 0, 1));
}

void EnvironmentMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//TODO: Sample ZED sensor readings. 
}


StatsMenu::StatsMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{

}

void StatsMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//TODO: Draw stats. 
}