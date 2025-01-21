#pragma once
#include <string>

namespace SCION_EDITOR
{
	struct SaveProject
	{
		/* The name of the project. */
		std::string sProjectName{ "" };
		/* The active path of the project. */
		std::string sProjectPath{ "" };
		/* The active project file path. */
		std::string sProjectFilePath{ "" };
		/* The main lua script path of the project. */
		std::string sMainLuaScript{ "" };
	};
}
