#include "pch.h"

#include "SGNode_FormatText.h"
#include <format>
#include <regex>

IMPLEMENT_GRAPH_NODE(SGNode_FormatText, ScriptGraphNode)

SGNode_FormatText::SGNode_FormatText()
{
    AddFlag(GraphNodeFlag_DynamicInputPins);

    CreateExecPin("In", PinDirection::Input, false);
    CreateExecPin("Out", PinDirection::Output, false);

    CreateDataPin<std::string>("Format", PinDirection::Input);
    CreateDataPin<std::string>("Result", PinDirection::Output);
}

NodeResult SGNode_FormatText::DoOperation()
{
    std::string pinValue;
    GetPinData("Format", pinValue);

    if (!pinValue.empty())
    {
        const std::regex expression(R"(\{[a-zA-Z0-9]*\})");
        std::vector<std::string> tokenPins = GetFormatTokens(pinValue);
        for (size_t p = 0; p < tokenPins.size(); p++)
        {
            std::string &tokenValue = tokenPins[p];
            if (!GetPinData(tokenPins[p], tokenValue))
            {
                tokenValue = "";
            }

            pinValue = std::regex_replace(pinValue, expression, tokenValue, std::regex_constants::format_first_only);
        }

        SetPinData("Result", pinValue);
    }

    return ExecPin("Out");
}

void SGNode_FormatText::OnUserChangedPinValue(ScriptGraphSchema *aSchema, size_t aPinId)
{
    const ScriptGraphPin &pin = GetPin(aPinId);
    if (pin.GetLabel() == "Format")
    {
        std::string pinValue;
        GetPinData("Format", pinValue);

        std::vector<std::string> pinsToCreate = GetFormatTokens(pinValue);
        std::vector<size_t> pinsToDelete;

        // Check if the pins already exist.
        for (const auto &[pinId, existingPin] : GetPins())
        {
            if (!existingPin.IsDynamicPin())
                continue;

            bool pinExists = false;
            for (auto pinToCreate = pinsToCreate.begin(); pinToCreate != pinsToCreate.end(); ++pinToCreate)
            {
                if (existingPin.GetLabel() == *pinToCreate)
                {
                    pinToCreate = pinsToCreate.erase(pinToCreate);
                    pinExists = true;
                    break;
                }
            }

            if (!pinExists)
            {
                pinsToDelete.emplace_back(pinId);
            }
        }

        for (const auto &pinToDelete : pinsToDelete)
        {
            aSchema->MarkDynamicPinForDelete(pinToDelete);
        }

        for (const auto &pinToCreate : pinsToCreate)
        {
            aSchema->CreateDynamicDataPin(this, pinToCreate, PinDirection::Input, typeid(std::string));
        }
    }
}

std::vector<std::string> SGNode_FormatText::GetFormatTokens(const std::string &aString) const
{
    std::vector<std::string> result;
    const std::regex expression(R"(\{[a-zA-Z0-9]*\})");
    std::smatch regexMatch;
    std::string::const_iterator it(aString.cbegin());
    while (std::regex_search(it, aString.cend(), regexMatch, expression))
    {
        std::string match = regexMatch[0];
        result.emplace_back(match.substr(1, match.length() - 2));
        it = regexMatch.suffix().first;
    }
    return result;
}
