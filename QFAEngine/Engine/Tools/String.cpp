#include "pch.h"
#include "String.h"

std::vector<QFAString::AnalyzeResult>& QFAString::AnalyzeString(const char* str)
{
	static std::vector<AnalyzeResult> ars;
	ars.clear();
	int offset = 0;

	while (true)
	{
		if (str[offset] == 0)
			break;

		try
		{
			AnalyzeResult as;
			as.Value = std::stof(std::string((char*)&str[offset]));;
			as.Action = ValueAction::None;
			while (true)
			{
				offset++;
				if (str[offset] == 0)
				{
					if (str[offset - 1] == '%')
						as.Type = StrValueType::Percent;
					else
						as.Type = StrValueType::Pixel;

					break;
				}

				if (str[offset] == ' ')
				{
					if (str[offset - 1] == '%')
						as.Type = StrValueType::Percent;
					else
						as.Type = StrValueType::Pixel;

					offset++;
					break;
				}
			}

			ars.push_back(as);
		}
		catch (const std::exception&)
		{
			AnalyzeResult as;
			as.Action = ValueAction::None;
			while (true)
			{
				if (str[offset] == 0)
					break;

				switch (str[offset])
				{
				case '+':	as.Action = ValueAction::Add; break;
				case '-':	as.Action = ValueAction::Minus; break;
				case '*':	as.Action = ValueAction::Multiply; break;
				case '/':	as.Action = ValueAction::Division;  break;
				}

				if (as.Action != ValueAction::None)
				{
					offset += 2;
					ars.push_back(as);
					break;
				}
				else
					offset++;
			}
		}
	}

	return ars;
}

float QFAString::GetValue(const char* str, int percentValue, bool is_Size)
{
    	std::vector<AnalyzeResult>& ars = AnalyzeString(str);
	if (!ars.size())
	{
		if (is_Size)
			return 1;

		return 0;
	}

	bool b = true;
	for (int i = 0; i < ars.size() - 1; i++) // check if string have valide data
	{ // 0 and even numbers is value, not even numbers is operation
		if (b)
		{
			if (!(ars[i].Action == ValueAction::None && ars[i + 1].Action > ValueAction::None))
				stopExecute("") // you can put wrong value like "100% -30" correct value be "100% - 30"
		}
		else
		{
			if (!(ars[i].Action > ValueAction::None && ars[i + 1].Action == ValueAction::None))
				stopExecute("")
		}

		b = !b;
	}

	int offset = 0;
	float value = ars[0].Value;
	if (ars[0].Type == StrValueType::Percent)
		value = (value / 100) * (float)percentValue;

	for (int i = 1; i < ars.size() - 1; i += 2)
	{
		AnalyzeResult& res = ars[i];
		AnalyzeResult& resV = ars[i + 1];
		if (resV.Type == StrValueType::Percent)
			resV.Value = (resV.Value / 100) * (float)percentValue;

		switch (res.Action)
		{
			case QFAString::Add:		value += resV.Value; break;
			case QFAString::Minus:		value -= resV.Value; break;
			case QFAString::Multiply:	value *= resV.Value; break;
			case QFAString::Division:	if (abs(resV.Value) > 0.00001)
													value /= resV.Value;

												break;
		}
	}

	return value;
}
