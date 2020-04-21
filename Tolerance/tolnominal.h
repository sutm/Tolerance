#pragma once

template <typename TDerived>
class HasNominal
{
public:
	using T = TDerived::Type;

	bool HasRelativeMode() const override { return true; }

	void SetNominal(T value) { m_dNominal = value; }
	T GetNominal() const { return m_dNominal; }

private:
	T m_dNominal;
	bool m_bRelative;
};