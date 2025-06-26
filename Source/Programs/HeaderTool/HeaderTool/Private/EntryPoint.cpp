#pragma once

#include "CmdParser.h"
#include "HeaderTool.h"

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	TArray<FString> Options = {"Source", "Sources", "Output", "ModuleName"};
	Shared::FCmdParser CmdParser(Argc, Argv, std::move(Options));

	FStringView SourcePath;
	FStringView OutputPath;
	FStringView ModuleName;

	HeaderTool::FHeaderTool HeaderTool;

	//CmdParser.Parse();
	//if (CmdParser.HasOptions({"Sources", "Output"}))
	//{
	//	SourcePath = CmdParser.GetOptionValue("Sources");
	//	OutputPath = CmdParser.GetOptionValue("Output");
	//	HeaderTool.Setup(SourcePath, OutputPath);
	//}
	//else if (CmdParser.HasOptions({"Source", "Output", "ModuleName"}))
	//{
	//	SourcePath = CmdParser.GetOptionValue("Source");
	//	OutputPath = CmdParser.GetOptionValue("Output");
	//	ModuleName = CmdParser.GetOptionValue("ModuleName");
	//	HeaderTool.Setup(SourcePath, OutputPath, ModuleName);
	//}
	//else
	//{
	//	CmdParser.BuildErrorMessage({{"Sources", "Output"}, {"Source", "Output", "ModuleName"}});
	//	BS_LOG(Fatal, "{}", CmdParser.GetErrorMessage());
	//}
	HeaderTool.Setup("C:/Users/Pipka/Desktop/Bloodshot/Source/Runtime", "C:/Users/Pipka/Desktop/Bloodshot/Intermediate");
	HeaderTool.Launch();
}
