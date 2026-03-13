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

private:
	float r, g, b, a;
	float min = 0.f, max = 1.f;
};

inline Color PlayerHPColor()	{ return Color{ 0.0f,1.0f,0.0f }; }
inline Color EnemyHPColor()		{ return Color{ 1.0f,0.0f,0.0f }; }
inline Color ShieldColor()		{ return Color{ 1.0f,1.0f,0.0f }; }

#endif // ! COLOR_HPP
