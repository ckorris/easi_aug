//#include <Menus.h>
#include <Drawables.h>
#include <Config.h>
#include <Stats.h>



static bool sidebarToggled = false;
//ImageButton* openSidebarButton;
unique_ptr<ImageButton> openSidebarButton;

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

	bool(Sidebar::*checkselected)(int index);
	checkselected = &Sidebar::CheckSelected;

	//Open menu button. Note this one is not added to children; it's drawn separately. 
	void(*openSidebar)() = []() {sidebarToggled = true; };
	
	openSidebarButton = make_unique<ImageButton>(ImageButton(openSidebar, "../images/hamburger_icon.png", false, 0, 1.0, 0.0, 0.167));

	//Calibration.
	Drawable::children.emplace_back(new SidebarButton(0, this, "../images/calib_icon.png", 0.0, 1.0, 0.0, 0.167));
	//Projectile.
	Drawable::children.emplace_back(new SidebarButton(1, this, "../images/projectile_icon.png", 0.0, 1.0, 0.167, 0.333));
	//Display.
	Drawable::children.emplace_back(new SidebarButton(2, this, "../images/display_icon.png", 0.0, 1.0, 0.333, 0.5));
	//Environment.
	Drawable::children.emplace_back(new SidebarButton(3, this, "../images/environment_icon.png", 0.0, 1.0, 0.5, 0.667));
	//Stats.
	Drawable::children.emplace_back(new SidebarButton(4, this, "../images/stats_icon.png", 0.0, 1.0, 0.667, 0.833));

	//Close menu button.
	void(*closeSidebar)() = []() {sidebarToggled = false; };
	Drawable::children.emplace_back(new ImageButton(closeSidebar, "../images/hamburger_icon.png", false, 0.0, 1.0, 0.833, 1.0));
}

void Sidebar::ProcessUI(cv::Rect parentrect, cv::Mat drawto, string windowname)
{
	
	
	if (sidebarToggled == true)
	{
		//cv::Rect togglebuttonbounds = cv::Rect(screenBounds.x, screenBounds.y + screenBounds.height, 
		//	screenBounds.x, screenBounds.height / 5.0);

		Drawable::ProcessUI(parentrect, drawto, windowname); //Base. 

		//Process the UI for only the enabled menu.
		if (selectedIndex >= 0 && selectedIndex < menus.size())
		{
			//Draw background behind the menu.
			cv::rectangle(drawto, openPanelRect, BACKGROUND_COLOR, -1);

			//Draw the menu itself. 
			menus[selectedIndex]->ProcessUI(openPanelRect, drawto, windowname);
		}
	}
	else //isToggled == false
	{
		//openSidebarButton->ProcessUI(parentrect, drawto, windowname);
		openSidebarButton->ProcessUI(parentrect, drawto, windowname);
	}


	
}

void Sidebar::ProcessAllClicks(int x, int y, bool isdown)
{
	Drawable::ProcessAllClicks(x, y, isdown); //Base.

	//Process the clicks for only the enabled menu. 
	if (sidebarToggled && selectedIndex >= 0 && selectedIndex < menus.size())
	{
		menus[selectedIndex]->ProcessAllClicks(x, y, isdown);
	}
	else
	{
		openSidebarButton->ProcessAllClicks(x, y, isdown);
	}

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
	Drawable::children.emplace_back(new SettingIncrementorPanel(hopupgetter, hopupsetter, 2000, "HOP-UP", "%1.0f RPM", cv::Scalar(0, 0, 255, 1), 0.35, 0.65, 0, 1));

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
	Drawable::children.emplace_back(new ToggleButton(drawLaserCrosshairGetter, drawLaserCrosshairSetter, 0.125, 0.25, 0.1, 0.4));
	//Toggle gravity crosshair.
	bool(*drawGravityCrosshairGetter)() = []() { return Config::toggleGravityCrosshair(); };
	void(*drawGravityCrosshairSetter)(bool) = [](bool b) { Config::toggleGravityCrosshair(b); };
	Drawable::children.emplace_back(new Label("G-XHR:", 0.0, 0.125, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(drawGravityCrosshairGetter, drawGravityCrosshairSetter, 0.125, 0.25, 0.6, 0.9));
	//Toggle laser path.
	bool(*drawLaserPathGetter)() = []() { return Config::toggleLaserPath(); };
	void(*drawLaserPathSetter)(bool) = [](bool b) { Config::toggleLaserPath(b); };
	Drawable::children.emplace_back(new Label("L-PTH:", 0.25, 0.375, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawLaserPathGetter, drawLaserPathSetter, 0.375, 0.5, 0.1, 0.4));
	//Toggle gravity path.q
	bool(*drawGravityPathGetter)() = []() { return Config::toggleGravityPath(); };
	void(*drawGravityPathSetter)(bool) = [](bool b) { Config::toggleGravityPath(b); };
	Drawable::children.emplace_back(new Label("G-PTH:", 0.25, 0.375, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(drawGravityPathGetter, drawGravityPathSetter, 0.375, 0.5, 0.6, 0.9));
	//Toggle distance.
	bool(*drawDistanceGetter)() = []() { return Config::toggleDistance(); };
	void(*drawDistanceSetter)(bool) = [](bool b) { Config::toggleDistance(b); };
	Drawable::children.emplace_back(new Label("DIST:", 0.5, 0.625, 0, 0.5));
	Drawable::children.emplace_back(new ToggleButton(drawDistanceGetter, drawDistanceSetter, 0.625, 0.75, 0.1, 0.4));
	//Toggle travel time.
	bool(*travelTimeGetter)() = []() { return Config::toggleTravelTime(); };
	void(*travelTimeSetter)(bool) = [](bool b) { Config::toggleTravelTime(b); };
	Drawable::children.emplace_back(new Label("TIME:", 0.5, 0.625, 0.5, 1));
	Drawable::children.emplace_back(new ToggleButton(travelTimeGetter, travelTimeSetter, 0.625, 0.75, 0.6, 0.9));


	//Rotate buttons.
	//Screen rotation. Increments the screen rotation by one, but after 3 it goes back to zero. 
	void(*displayrotator)() = []() { Config::screenRotation((Config::screenRotation() + 1) % 4); };
	Drawable::children.emplace_back(new ImageButton(displayrotator, "../images/display_rotate_icon.png", true, 0.8, 1, 0, 0.5));
	//Image rotation. Increments the image rotation by one, but after 3 it goes back to zero. 
	void(*imagerotator)() = []() { Config::imageRotation((Config::imageRotation() + 1) % 4); };
	Drawable::children.emplace_back(new ImageButton(imagerotator, "../images/cam_rotate_icon.png", true, 0.8, 1, 0.5, 1));
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
	void(*relhumidsetter)(float) = [](float v) {Config::relativeHumidity01(v); }; //TODO: String format below shows percentage, but also decimal, eg. 0.77%. Should be 77%.
	Drawable::children.emplace_back(new SettingIncrementorPanel(relhumidgetter, relhumidsetter, 0.01, "REL. HUMID", "%1.2f%%", cv::Scalar(0, 0, 255, 1), 0.35, 0.65, 0, 1));

	//Stats readouts related to the air. 
	//Air pressure from the ZED's barometer.
	float(*airpressuregetter)() = []() {return Stats::GetLatestAirPressure(); };
	Drawable::children.emplace_back(new ValueLabel(airpressuregetter, "Pressure: ", "%1.2f HPA", 0.69, 1, 0, 0.334));

	//Air density in KG/m3. 
	float(*airdensitygetter)() = []() {return Stats::GetLatestAirDensity(); };
	Drawable::children.emplace_back(new ValueLabel(airdensitygetter, "Density: ", "%1.2f kg/m3", 0.69, 1, 0.334, 0.667));

	//Air viscosity in centiPoise. Note the multiplication in the getter, which converts from the Poise figure retrieved from Stats. 
	float(*airviscositygetter)() = []() {return Stats::GetLatestAirViscosity() * 100; };
	Drawable::children.emplace_back(new ValueLabel(airviscositygetter, "Viscosity: ", "%1.6f cP", 0.69, 1, 0.667, 1));
}

void EnvironmentMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//TODO: Sample ZED sensor readings. 
}


StatsMenu::StatsMenu(float anchorxmin, float anchorxmax, float anchorymin, float anchorymax)
	: Drawable::Drawable(anchorxmin, anchorxmax, anchorymin, anchorymax)
{
	//First column: Speed. 
	//Barrel speed. This is directly from the config and viewable elsewhere but is just for convenience. 
	float(*speedbarrelgetter)() = []() {return Config::forwardSpeedMPS(); };
	Drawable::children.emplace_back(new ValueLabel(speedbarrelgetter, "Speed @Brl: ", "%1.1f mps", 0.0, 0.334, 0.0, 0.5));

	//Impact speed. 
	float(*speedimpactgetter)() = []() {return Stats::GetLatestSpeedImpact(); };
	Drawable::children.emplace_back(new ValueLabel(speedimpactgetter, "Speed @Imp: ", "%1.1f mps", 0.0, 0.334, 0.5, 1));

	//Second column: Drag. 
	//Drag coefficient at barrel. 
	float(*dragcoefbarrelgetter)() = []() {return Stats::GetDragCoefficientBarrel(); };
	Drawable::children.emplace_back(new ValueLabel(dragcoefbarrelgetter, "cD @Brl: ", "%1.4f", 0.334, 0.667, 0.0, 0.25));
	//Drag coefficient at impact. 
	float(*dragcoefimpactgetter)() = []() {return Stats::GetDragCoefficientImpact(); };
	Drawable::children.emplace_back(new ValueLabel(dragcoefimpactgetter, "cD @Imp: ", "%1.4f", 0.334, 0.667, 0.25, 0.5));
	//Drag force at barrel. 
	float(*dragforcebarrelgetter)() = []() {return Stats::GetDragForceBarrel(); };
	Drawable::children.emplace_back(new ValueLabel(dragforcebarrelgetter, "fD @Brl: ", "%1.4f", 0.334, 0.667, 0.5, 0.75));
	//Drag force at barrel. 
	float(*dragforceimpactgetter)() = []() {return Stats::GetDragForceImpact(); };
	Drawable::children.emplace_back(new ValueLabel(dragforceimpactgetter, "fD @Imp: ", "%1.4f", 0.334, 0.667, 0.75, 1.0));

	//Third column: Magnus/Lift/Hop-Up. 
	//Lift coefficient at barrel. 
	float(*magnuscoefbarrelgetter)() = []() {return Stats::GetMagnusCoefficientBarrel(); };
	Drawable::children.emplace_back(new ValueLabel(magnuscoefbarrelgetter, "cL @Brl: ", "%1.4f", 0.667, 1.0, 0.0, 0.25));
	//Lift coefficient at impact. 
	float(*magnuscoefimpactgetter)() = []() {return Stats::GetMagnusCoefficientImpact(); };
	Drawable::children.emplace_back(new ValueLabel(magnuscoefimpactgetter, "cL @Imp: ", "%1.4f", 0.667, 1.0, 0.25, 0.5));
	//Lift force at barrel. 
	float(*magnusforcebarrelgetter)() = []() {return Stats::GetMagnusForceBarrel(); };
	Drawable::children.emplace_back(new ValueLabel(magnusforcebarrelgetter, "fL @Brl: ", "%1.4f", 0.667, 1.0, 0.5, 0.75));
	//Lift force at barrel. 
	float(*magnusforceimpactgetter)() = []() {return Stats::GetMagnusForceImpact(); };
	Drawable::children.emplace_back(new ValueLabel(magnusforceimpactgetter, "fL @Imp: ", "%1.4f", 0.667, 1.0, 0.75, 1.0));
}

void StatsMenu::Draw(cv::Rect drawrect, cv::Mat drawto, string windowname)
{
	//TODO: Draw stats. 
}