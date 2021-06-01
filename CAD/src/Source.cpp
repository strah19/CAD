#include "Core/Application.h"
#include "Gui/Camera.h"

class Shape {
public:
	void Initialize(const Ember::IVec2& click) {
		initial_click_point = click;
	}
	virtual ~Shape() = default;
	virtual void Update(Ember::Camera& camera, Ember::Events* events) = 0;
	virtual void Draw(Ember::Camera& camera, Ember::rRenderer* renderer) = 0;
protected:
	Ember::IVec2 initial_click_point;
};

class Rect : public Shape {
public:
	void Update(Ember::Camera& camera, Ember::Events* events) override {
		float x, y;
		camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
		size = Ember::IVec2((int)x, (int)y) - initial_click_point;
	}

	void Draw(Ember::Camera& camera, Ember::rRenderer* renderer) override {
		int sx, sy;
		camera.WorldToScreen((float)initial_click_point.x, (float)initial_click_point.y, sx, sy);
		renderer->Rectangle({ sx, sy, size.x, size.y }, { 0, 255, 255, 255 });
	}
private:
	Ember::IVec2 size;
};

class Line : public Shape {
public:
	void Update(Ember::Camera& camera, Ember::Events* events) override {
		float x, y;
		camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
		end_spot = Ember::IVec2((int)x, (int)y) - initial_click_point;
	}

	void Draw(Ember::Camera& camera, Ember::rRenderer* renderer) override {
		int sx, sy;
		camera.WorldToScreen((float)initial_click_point.x, (float)initial_click_point.y, sx, sy);
		renderer->Line({ sx, sy }, { sx + end_spot.x, sy + end_spot.y }, { 255, 255, 0, 255 });
	}
private:
	Ember::IVec2 end_spot;
};

class Circle : public Shape {
public:
	void Update(Ember::Camera& camera, Ember::Events* events) override {
		float x, y;
		camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
		radius = (int)x - initial_click_point.x;
	}

	void Draw(Ember::Camera& camera, Ember::rRenderer* renderer) override {
		int sx, sy;
		camera.WorldToScreen((float)initial_click_point.x, (float)initial_click_point.y, sx, sy);
		renderer->DrawCircle({ sx, sy }, radius, { 255, 255, 255, 255 });
	}
private:
	int radius;
};

class CircleFill : public Shape {
public:
	void Update(Ember::Camera& camera, Ember::Events* events) override {
		float x, y;
		camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
		radius = (int)x - initial_click_point.x;
	}

	void Draw(Ember::Camera& camera, Ember::rRenderer* renderer) override {
		int sx, sy;
		camera.WorldToScreen((float)initial_click_point.x, (float)initial_click_point.y, sx, sy);
		renderer->FillCircle({ sx, sy }, radius, { 255, 255, 255, 255 });
	}
private:
	int radius;
};

class Curve : public Shape {
public:
	void Update(Ember::Camera& camera, Ember::Events* events) override {
		float x, y;
		camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
		second_point = Ember::IVec2((int)x, (int)y) - initial_click_point;
	}

	void Draw(Ember::Camera& camera, Ember::rRenderer* renderer) override {
		int sx, sy;
		camera.WorldToScreen((float)initial_click_point.x, (float)initial_click_point.y, sx, sy);
		
		Ember::IVec2 arr[3];
		arr[0] = { 0, 0 };
		arr[1] = initial_click_point;
		arr[2] = second_point;
		renderer->Curve(arr, { 255, 255, 255, 255 });
	}
private:
	Ember::IVec2 second_point;
};

class CAD : public Ember::Application {
public:
	void OnCreate() { }

	virtual ~CAD() { }

	void OnUserUpdate() {
		window->Update();

		camera.Pan(Ember::ButtonIds::MiddleMouseButton, events);

		renderer->Clear(background_color);

		if (!shapes.empty() && events->Down() && (events->ButtonId() == Ember::ButtonIds::LeftMouseButton || events->ButtonId() == Ember::ButtonIds::RightMouseButton))
			shapes.back()->Update(camera, events);

		for (auto& shape : shapes) 
			shape->Draw(camera, renderer);

		renderer->Show();
	}

	bool Keyboard(Ember::KeyboardEvents& keyboard) {
		if (keyboard.scancode == Ember::EmberKeyCode::Escape) {
			window->Quit();
			return true;
		}
		return false;
	}

	bool MouseButton(Ember::MouseButtonEvents& mouse) {
		if (mouse.clicked && mouse.button_id == Ember::ButtonIds::LeftMouseButton && events->KeyCode() == Ember::EmberKeyCode::LeftCtrl && events->KeyDown()) {
			shapes.push_back(new Circle());

			float x, y;
			camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
			shapes.back()->Initialize({ (int)x, (int)y });
		}
		else if (mouse.clicked && mouse.button_id == Ember::ButtonIds::LeftMouseButton && events->KeyCode() == Ember::EmberKeyCode::LeftShift && events->KeyDown()) {
			shapes.push_back(new CircleFill());

			float x, y;
			camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
			shapes.back()->Initialize({ (int)x, (int)y });
		}
		else if (mouse.clicked && mouse.button_id == Ember::ButtonIds::LeftMouseButton && events->KeyCode() == Ember::EmberKeyCode::LeftAlt && events->KeyDown()) {
			shapes.push_back(new Curve());

			float x, y;
			camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
			shapes.back()->Initialize({ (int)x, (int)y });
		}
		else if (mouse.clicked && mouse.button_id == Ember::ButtonIds::LeftMouseButton) {
			shapes.push_back(new Rect());

			float x, y;
			camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
			shapes.back()->Initialize({ (int)x, (int)y });
		}
		else if (mouse.clicked && mouse.button_id == Ember::ButtonIds::RightMouseButton) {
			shapes.push_back(new Line());

			float x, y;
			camera.ScreenToWorld(events->MousePosition().x, events->MousePosition().y, x, y);
			shapes.back()->Initialize({ (int)x, (int)y });
		}
		return true;
	}

	void UserDefEvent(Ember::Event& event) {
		Ember::EventDispatcher dispatch(&event);
		dispatch.Dispatch<Ember::KeyboardEvents>(EMBER_BIND_FUNC(Keyboard));
		dispatch.Dispatch<Ember::MouseButtonEvents>(EMBER_BIND_FUNC(MouseButton));
	}
private:
	Ember::Color background_color = { 0, 0, 0, 255 };
	Ember::Camera camera;
	
	std::vector<Shape*> shapes;
};

int main(int argc, char** argv) {
	CAD cad;
	cad.Initialize("CAD");

	cad.Run();

	return 0;
}