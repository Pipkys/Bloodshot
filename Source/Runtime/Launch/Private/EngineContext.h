#pragma once

#include "Core.h"

#include "CmdParser.h"

namespace Bloodshot::Private::Launch
{
	class IEngineContext
	{
	public:
		IEngineContext() = default;
		IEngineContext(const IEngineContext&) = delete;
		IEngineContext& operator=(const IEngineContext&) = delete;
		virtual ~IEngineContext() {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) = 0;
		virtual void Init() = 0;
		virtual void Run() = 0;
		virtual void Exit() = 0;

		void PreInit();
		void Dispose();

		void BeginPlay();
		void EndPlay();
	};
}

// FEditor Editor

//class FGameDLLObserver
//{
//public:
//	void SetTimestampPath(const std::filesystem::path& TimestampPath);
//	void Observe();
//
//private:
//	std::string TimestampPath;
//	std::string OldTimestamp;
//	std::string NewTimestamp;
//	std::ifstream InputStream;
//};
//
//FSettings Settings;
//FGameDLLObserver GameDLLObserver;

//void FEditor::FGameDLLObserver::SetTimestampPath(const std::filesystem::path& TimestampPath)
//{
//	this->TimestampPath = TimestampPath.string();
//}
//
//void FEditor::FGameDLLObserver::Observe()
//{
//	InputStream.open(TimestampPath);
//	std::getline(InputStream, NewTimestamp);
//
//	if (OldTimestamp != NewTimestamp)
//	{
//		OldTimestamp = NewTimestamp;
//
//		// Reload game dll...
//		BS_LOG(Info, "Reloading game dll...");
//	}
//
//	InputStream.close();
//}
