#include "Generator.h"

#include <fstream>

namespace Bloodshot::HeaderTool
{
	namespace Private
	{
		static const FClassInfo* FindClass(const FString& ClassName, const TArray<FHeaderFileInfo>& HeadersInfo)
		{
			for (const FHeaderFileInfo& HeaderInfo : HeadersInfo)
			{
				for (const FClassInfo& ClassInfo : HeaderInfo.ClassInfos)
				{
					if (ClassInfo.Name == ClassName)
					{
						return &ClassInfo;
					}
				}
			}

			return nullptr;
		}

		static void CalculateSizeOfClassRecursive(size_t& OutBaseClassCount,
			size_t& OutPropertyCount,
			size_t& OutFunctionCount,
			size_t& OutParameterCount,
			const FClassInfo& ClassInfo,
			const TArray<FHeaderFileInfo>& HeadersInfo)
		{
			OutBaseClassCount += ClassInfo.BaseClassNames.GetSize();
			OutPropertyCount += ClassInfo.Properties.GetSize();
			OutFunctionCount += ClassInfo.Functions.GetSize();
			for (const FFunctionInfo& Function : ClassInfo.Functions)
			{
				OutParameterCount += Function.Parameters.GetSize();
			}
			for (const FString& BaseClassName : ClassInfo.BaseClassNames)
			{
				const FClassInfo* BaseClass = FindClass(BaseClassName, HeadersInfo);
				if (BaseClass)
				{
					CalculateSizeOfClassRecursive(OutBaseClassCount,
						OutPropertyCount,
						OutFunctionCount,
						OutParameterCount,
						*BaseClass,
						HeadersInfo);
				}
			}
		}
	}

	FGeneratorOutput FGenerator::Generate(const FHeaderFileInfo& HeaderInfo, const TArray<FHeaderFileInfo>& HeadersInfo)
	{
		Clear();
		CurrentOutput = &SourceOutput;
		WriteLine("// Generated code by HeaderTool");
		WriteLine("// Do not modify this manually!");
		WriteLine("#include \"Object/ObjectGlobals.h\"");
		WriteLine("#include \"{}\"", HeaderInfo.Path);
		WriteLine("namespace Bloodshot");
		WriteLine("{");
		PushScope();
		WriteClassConstructionImpl(HeaderInfo, HeadersInfo);
		PopScope();
		WriteLine("}");

		return FGeneratorOutput(SourceOutput, HeaderOutput);
	}

	void FGenerator::WriteLine(FStringView String)
	{
		for (size_t i = 0; i < PushedScopes; ++i)
		{
			CurrentOutput->Append("\t");
		}

		CurrentOutput->Append(String).Append("\n");
	}

	void FGenerator::PushScope()
	{
		++PushedScopes;
	}

	void FGenerator::PopScope()
	{
		BS_ASSERT(PushedScopes, "FGenerator::PopScope: called when PushedScopes == 0");
		--PushedScopes;
	}

	void FGenerator::Clear()
	{
		SourceOutput.Clear();
		HeaderOutput.Clear();

		CurrentOutput = nullptr;
		PushedScopes = 0;
	}

	void FGenerator::WriteClassConstructionImpl(const FHeaderFileInfo& HeaderInfo, const TArray<FHeaderFileInfo>& HeadersInfo)
	{
		for (const FClassInfo& ClassInfo : HeaderInfo.ClassInfos)
		{
			static uint32_t ClassUniqueID = 1;
			const FString& QualifiedClassName = ClassInfo.Namespace + "::" + ClassInfo.Name;

			WriteLine("namespace Private");
			WriteLine("{");
			PushScope();

			for (const FFunctionInfo& Function : ClassInfo.Functions)
			{
				WriteLine("template<IsObject ObjectType, typename... ArgTypes>");
				FString Buffer;
				Buffer += String::Format("struct Object::IFunctionCaller<{}, {} ({}::*)(", QualifiedClassName, Function.ReturnType, QualifiedClassName);
				for (const FParameterInfo& Parameter : Function.Parameters)
				{
					Buffer += Parameter.Type;
					Buffer += ",";
				}
				if (Function.Parameters.GetSize())
				{
					Buffer.RemoveLast();
				}
				Buffer += "), ObjectType, ArgTypes...> final";

				WriteLine(Buffer);
				WriteLine("{");
				PushScope();
				WriteLine("static void Call(ObjectType* Object, void* Return, ArgTypes&&... Args)");
				WriteLine("{");
				PushScope();
				WriteLine("*({}*)Return = Object->{}(std::forward<ArgTypes>(Args)...);", Function.ReturnType, Function.Name);
				PopScope();
				WriteLine("}");
				PopScope();
				WriteLine("};");
			}

			WriteLine("template<>");
			WriteLine("FClass* Object::FObjectCore::ConstructClass<{}>({}* const InObject, void* const InMemory)", QualifiedClassName, QualifiedClassName);
			WriteLine("{");
			PushScope();

			size_t TotalBaseClassCount = 0;
			for (size_t i = 0; i < ClassInfo.BaseClassNames.GetSize(); ++i)
			{
				if (Private::FindClass(ClassInfo.BaseClassNames[i], HeadersInfo))
				{
					++TotalBaseClassCount;
				}
			}
			{
				size_t BaseClassCount = 0;
				size_t PropertyCount = 0;
				size_t FunctionCount = 0;
				size_t ParameterCount = 0;
				Private::CalculateSizeOfClassRecursive(BaseClassCount, PropertyCount, FunctionCount, ParameterCount, ClassInfo, HeadersInfo);
				WriteLine("void* const Memory = InMemory ? InMemory : FMemory::Malloc(sizeof(FClass) + sizeof(FClass) * {} + sizeof(FClass**) * {} + sizeof(FProperty) * {} + sizeof(FFunction) * {} + sizeof(FParameter) * {});",
					BaseClassCount,
					BaseClassCount,
					PropertyCount,
					FunctionCount,
					ParameterCount);
			}

			FString BaseClassOffsetsStr = String::Format("constexpr std::array<size_t, {}> BaseClassOffsets = {{0, ", TotalBaseClassCount + 1);
			if (TotalBaseClassCount)
			{
				for (const FString& BaseClassName : ClassInfo.BaseClassNames)
				{
					size_t BaseBaseClassCount = 0;
					size_t PropertyCount = 0;
					size_t FunctionCount = 0;
					size_t ParameterCount = 0;
					const FClassInfo* BaseClassInfo = Private::FindClass(BaseClassName, HeadersInfo);
					if (BaseClassInfo)
					{
						Private::CalculateSizeOfClassRecursive(BaseBaseClassCount, PropertyCount, FunctionCount, ParameterCount, *BaseClassInfo, HeadersInfo);
						BaseClassOffsetsStr += String::Format("sizeof(FClass) + sizeof(FClass) * {} + sizeof(FClass**) * {} + sizeof(FProperty) * {} + sizeof(FFunction) * {} + sizeof(FParameter) * {},",
							BaseBaseClassCount,
							BaseBaseClassCount,
							PropertyCount,
							FunctionCount,
							ParameterCount);
					}
				}
				BaseClassOffsetsStr.RemoveLast();
				BaseClassOffsetsStr += "};";
			}
			else
			{
				BaseClassOffsetsStr = "constexpr std::array<size_t, 0> BaseClassOffsets;";
			}

			WriteLine(BaseClassOffsetsStr);
			WriteLine("FClass* const Class = (FClass*)Memory;");
			WriteLine("Class->Name = \"{}\";", ClassInfo.Name);
			WriteLine("Class->Namespace = \"{}\";", ClassInfo.Namespace);
			WriteLine("Class->bAbstract = {};", ClassInfo.bAbstract);
			WriteLine("Class->bFinal = {};", ClassInfo.bFinal);
			WriteLine("Class->bDerived = {};", ClassInfo.bDerived);
			WriteLine("Class->Size = sizeof({});", QualifiedClassName);
			WriteLine("Class->TypeID = {};", ClassUniqueID);

			WriteLine("size_t BaseClassIndex = 0;");
			WriteLine("FClass** BaseClassPtr = (FClass**)(Class + 1);");
			WriteLine("Class->BaseClasses = BaseClassPtr;");
			WriteLine("Class->BaseClassCount = {};", TotalBaseClassCount);

			WriteLine("FClass* BaseClassesPtr = (FClass*)(((std::byte*)Class) + sizeof(FClass) + sizeof(FClass*) * {});", TotalBaseClassCount);
			for (size_t i = 0; i < TotalBaseClassCount; ++i)
			{
				WriteLine("BaseClassPtr[{}] = (FClass*)(((std::byte*)(BaseClassesPtr)) + BaseClassOffsets[{}]);", i, i);
			}

			WriteLine("FClass* BaseClassLocalPtr = *BaseClassPtr;");
			for (const FString& BaseClassName : ClassInfo.BaseClassNames)
			{
				WriteLine("TryConstructOrDefaultClass<{}>(({}*)InObject, \"{}\", BaseClassLocalPtr);",
					BaseClassName,
					BaseClassName,
					BaseClassName);
				WriteLine("++BaseClassLocalPtr;");
			}

			WriteLine("FProperty* Properties = (FProperty*)(((std::byte*)BaseClassesPtr) + BaseClassOffsets.back());");
			WriteLine("Class->Properties = Properties;");
			WriteLine("Class->PropertyCount = {};", ClassInfo.Properties.GetSize());

			WriteLine("FProperty* PropertiesPtr = Properties;");
			for (const FPropertyInfo& PropertyInfo : ClassInfo.Properties)
			{
				WriteLine("new(PropertiesPtr) FProperty(");
				PushScope();
				WriteLine("\"{}\",", PropertyInfo.Type);
				WriteLine("\"{}\",", PropertyInfo.Name);
				WriteLine("{},", PropertyInfo.bStatic);
				//WriteLine("{},", PropertyInfo.bSerialized);
				//WriteLine("{},", PropertyInfo.bReplicated);
				WriteLine("(void*)&InObject->{});", PropertyInfo.Name);
				PopScope();
				WriteLine("++PropertiesPtr;");
			}

			WriteLine("FFunction* Functions = (FFunction*)((((std::byte*)BaseClassesPtr) + BaseClassOffsets.back()) + sizeof(FProperty) * {});", ClassInfo.Properties.GetSize());
			WriteLine("Class->Functions = Functions;");
			WriteLine("Class->FunctionCount = {};", ClassInfo.Functions.GetSize());

			WriteLine("FFunction* FunctionsPtr = Functions;");
			size_t FunctionNumber = 1;
			for (const FFunctionInfo& FunctionInfo : ClassInfo.Functions)
			{
				WriteLine("FParameter* Parameters{} = (FParameter*)(((std::byte*)(Functions + {})) + sizeof(FParameter) * {});", FunctionNumber, FunctionNumber, FunctionInfo.Parameters.GetSize());
				WriteLine("FParameter* ParametersPtr{} = Parameters{};", FunctionNumber, FunctionNumber);
				for (const FParameterInfo& ParameterInfo : FunctionInfo.Parameters)
				{
					WriteLine("new(ParametersPtr{}) FParameter(", FunctionNumber);
					PushScope();
					WriteLine("\"{}\",", ParameterInfo.Type);
					WriteLine("\"{}\");", ParameterInfo.Name);
					PopScope();
					WriteLine("++ParametersPtr{};", FunctionNumber);
				}

				WriteLine("new(FunctionsPtr) FFunction(");
				PushScope();
				WriteLine("\"{}\",", FunctionInfo.ReturnType);
				WriteLine("\"{}\",", FunctionInfo.Name);
				WriteLine("Parameters{},", FunctionNumber);
				WriteLine("{},", FunctionInfo.Parameters.GetSize());
				WriteLine("{},", FunctionInfo.bStatic);
				WriteLine("{},", FunctionInfo.bConst);
				WriteLine("{},", FunctionInfo.bNoexcept);




//			[](IObject* Object, FFunctionParams* Params)
//			{
//				Object::IFunctionCaller<::Bloodshot::Test::FTestObject,
//				size_t(::Bloodshot::Test::FTestObject::*)(const size_t),
//				::Bloodshot::Test::FTestObject,
//				const size_t>::Call((::Bloodshot::Test::FTestObject*)Object, std::move(*(const size_t*)Params->Value));
//			});



				FString Buffer = String::Format("[](IObject* Object, FFunctionParams* Params, void* Return){{IFunctionCaller<{}, {} ({}::*)(", QualifiedClassName, FunctionInfo.ReturnType, QualifiedClassName);
				if (FunctionInfo.Parameters.GetSize())
				{
					for (size_t i = 0; i < FunctionInfo.Parameters.GetSize(); ++i)
					{
						Buffer += FunctionInfo.Parameters[i].Type;
						Buffer += ", ";
					}
					Buffer.RemoveLast(2);
				}
				Buffer += String::Format("), {}, ", QualifiedClassName);
				if(FunctionInfo.Parameters.GetSize())
				{
					for (size_t i = 0; i < FunctionInfo.Parameters.GetSize(); ++i)
					{
						Buffer += FunctionInfo.Parameters[i].Type;
						Buffer += ", ";
					}
					Buffer.RemoveLast(2);
				}
				Buffer += String::Format(">::Call(({}*)Object, Return, ", QualifiedClassName);
				for (size_t i = 0; i < FunctionInfo.Parameters.GetSize(); ++i)
				{
					Buffer += String::Format("std::move(*(std::decay_t<{}>*)Params->", FunctionInfo.Parameters[i].Type);
					for (size_t j = 0; j < i; ++j)
					{
						Buffer += "Next->";
					}
					Buffer += "Value),";
				}
				Buffer.RemoveLast();
				Buffer += ");});";

				WriteLine(Buffer);
				PopScope();

				WriteLine("++FunctionsPtr;");
				++FunctionNumber;
			}

			WriteLine("return Class;");
			PopScope();
			WriteLine("}");
			PopScope();
			WriteLine("}");

			WriteLine("FClass* {}::GetPrivateStaticClass()", QualifiedClassName);
			WriteLine("{");
			PushScope();
			WriteLine("static FClass Instance(");
			PushScope();
			WriteLine("\"{}\",", ClassInfo.Name);
			WriteLine("\"{}\",", ClassInfo.Namespace);
			WriteLine("nullptr,");
			WriteLine("0,");
			WriteLine("nullptr,");
			WriteLine("0,");
			WriteLine("nullptr,");
			WriteLine("0,");
			WriteLine("{},", ClassInfo.bAbstract);
			WriteLine("{},", ClassInfo.bFinal);
			WriteLine("{},", ClassInfo.bDerived);
			WriteLine("sizeof({}),", QualifiedClassName);
			WriteLine("{});", ClassUniqueID);
			PopScope();
			WriteLine("return &Instance;");
			PopScope();
			WriteLine("}");

			++ClassUniqueID;
		}
	}

	void FGenerator::WriteSerializeionImpl()
	{

	}

	void FGenerator::WriteReplicationImpl()
	{

	}
}
