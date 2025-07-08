#pragma once
#include <string>
#include <memory>

namespace SCION_EDITOR
{
class IconReplacer
{
  public:
	IconReplacer( const std::string& sIconFile, const std::string& sExeFile );
	~IconReplacer();

	bool ReplaceIcon();

  private:
	class Impl;
	std::unique_ptr<Impl> m_pImpl;
};
} // namespace SCION_EDITOR
