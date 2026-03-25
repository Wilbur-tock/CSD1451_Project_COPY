#ifndef COLOR_HPP
#define COLOR_HPP

class Color {
public:
	Color();
	Color(float _r, float _g, float _b, float _a = 1.0f);

	// Accessor

	float Red() const { return r; }
	float Green() const { return g; }
	float Blue() const { return b; }
	float Alpha() const { return a; }

	void EditColorPercentage(float percentage);
	void SetColor(float _r, float _g, float _b, float _a = 1.0f);
	void SetColor(Color const& _color);

	void SetAlpha(float newAlpha) { a = newAlpha; }

private:
	float r, g, b, a;
	float min = 0.f, max = 1.f;
};

Color const& PlayerHPColor();
Color const& EnemyHPColor();
Color const& ShieldColor();
Color const& PanelColor();

#endif // ! COLOR_HPP
