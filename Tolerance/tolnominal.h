#pragma once

template <typename Traits>
class HasNominal
{
public:
	using T = typename Traits::Type;

	bool HasRelativeMode() const { return true; }

	void SetNominal(T value) { m_dNominal = value; }
	T GetNominal() const { return m_dNominal; }

private:
	T m_dNominal;
	bool m_bRelative;
};