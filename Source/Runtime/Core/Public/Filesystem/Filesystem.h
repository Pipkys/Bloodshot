#pragma once

#include "Containers/StringView.h"
#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"

#include <filesystem>

namespace Bloodshot
{
	class FFilesystem final
	{
	public:
		static bool CreateDirectories(FStringView InPath, std::error_code& OutErrorCode)
		{
			if (InPath.IsEmpty())
			{
				OutErrorCode = MakeErrorCode(__std_win_error::_Path_not_found);
				return false;
			}

			OutErrorCode.clear();

			const char* Cursor = InPath.GetData();
			const char* const End = InPath.GetData() + InPath.GetSize();
			const char* RootPathEnd = FindRelativePath(Cursor, End);
			if (RootPathEnd != Cursor
				&& End - RootPathEnd >= 3
				&& IsDrivePrefix(RootPathEnd)
				&& IsSlash(RootPathEnd[2]))
			{
				RootPathEnd += 2;
			}

			FString Temp;
			Temp.Reserve(InPath.GetSize());
			Temp.AppendChars(Cursor, RootPathEnd - Cursor);
			Cursor = RootPathEnd;

			bool CreatedLast = false;
			__std_win_error Error = __std_win_error::_Success;
			__std_win_error MostRecentNotFileNotFound = __std_win_error::_Success;
			while (Cursor != End)
			{
				const char* const AddedEnd = std::find_if(std::find_if_not(Cursor, End, IsSlash), End, IsSlash);
				Temp.AppendChars(Cursor, AddedEnd - Cursor);
				wchar_t* NewDirectoryData = (wchar_t*)FMemory::Allocate(Temp.GetSize(), EAllocationType::Temporary);
				memset(NewDirectoryData, 0, Temp.GetSize());
				std::mbstowcs(NewDirectoryData, Temp.GetData(), Temp.GetSize());
				NewDirectoryData[Temp.GetSize()] = '\0';
				const __std_fs_create_directory_result _Create_result = __std_fs_create_directory(NewDirectoryData);
				Error = _Create_result._Error;
				CreatedLast = _Create_result._Created;
				if (Error != __std_win_error::_Success && !__std_is_file_not_found(Error))
				{
					MostRecentNotFileNotFound = Error;
				}

				Cursor = AddedEnd;
			}

			if (Error != __std_win_error::_Success && MostRecentNotFileNotFound != __std_win_error::_Success)
			{
				Error = MostRecentNotFileNotFound;
			}

			OutErrorCode = MakeErrorCode(Error);
			return CreatedLast;
		}

		FORCEINLINE static bool CreateDirectories(FStringView InPath)
		{
			std::error_code ErrorCode;
			const bool Result = CreateDirectories(InPath, ErrorCode);
			BS_LOG_IF(ErrorCode, Fatal, "FFilesystem::CreateDirectories: {}", std::_Syserror_map(ErrorCode.value()));
			return Result;
		}

		NODISCARD FORCEINLINE static bool HasExtension(FStringView InPath)
		{
			return !ParseExtension(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static bool HasRootName(FStringView InPath)
		{
			return !ParseRootName(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static bool HasRootDirectory(FStringView InPath)
		{
			return !ParseRootDirectory(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static bool HasRootPath(FStringView InPath)
		{
			return !ParseRootPath(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static bool HasRelativePath(FStringView InPath)
		{
			return !ParseRelativePath(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static bool HasParentPath(FStringView InPath)
		{
			return !ParseParentPath(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static bool HasFilename(FStringView InPath)
		{
			return !ParseFilename(InPath).IsEmpty();
		}

		NODISCARD FORCEINLINE static FStringView GetExtension(FStringView InPath)
		{
			return ParseExtension(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetRootName(FStringView InPath)
		{
			return ParseRootName(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetRootDirectory(FStringView InPath)
		{
			return ParseRootDirectory(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetRootPath(FStringView InPath)
		{
			return ParseRootPath(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetRelativePath(FStringView InPath)
		{
			return ParseRelativePath(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetParentPath(FStringView InPath)
		{
			return ParseParentPath(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetFilename(FStringView InPath)
		{
			return ParseFilename(InPath);
		}

		NODISCARD FORCEINLINE static FStringView GetBaseFilename(FStringView InPath)
		{
			FStringView Filename = ParseFilename(InPath);
			FStringView Extension = ParseExtension(Filename);
			return FStringView(Filename.GetData(), Filename.GetSize() - Extension.GetSize());
		}

		NODISCARD FORCEINLINE static bool IsAbsolute(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			const char* const Last = First + InPath.GetSize();
			if (HasDriveLetterPrefix(First, Last))
			{
				return Last - First >= 3 && IsSlash(First[2]);
			}
			return First != FindRootNameEnd(First, Last);
		}

		NODISCARD FORCEINLINE static bool IsRelative(FStringView InPath)
		{
			return !IsAbsolute(InPath);
		}

		NODISCARD static FString GetCurrentPath(std::error_code& OutErrorCode)
		{
			OutErrorCode.clear();
			wchar_t* CurrentPathDataWide = (wchar_t*)FMemory::Allocate(MaxPathSize, EAllocationType::Temporary);
			const __std_ulong_and_error TempResult = __std_fs_get_current_path(MaxPathSize, CurrentPathDataWide);
			BS_CHECK(TempResult._Size < MaxPathSize);
			OutErrorCode = MakeErrorCode(TempResult._Error);
			char* CurrentPathData = (char*)FMemory::Allocate(MaxPathSize, EAllocationType::Temporary);
			wcstombs(CurrentPathData, CurrentPathDataWide, TempResult._Size);
			return FString(CurrentPathData, TempResult._Size);
		}

		NODISCARD static size_t GetCurrentPath(char* OutCurrentPath, std::error_code& OutErrorCode)
		{
			OutErrorCode.clear();
			wchar_t* CurrentPathDataWide = (wchar_t*)FMemory::Allocate(MaxPathSize, EAllocationType::Temporary);
			const __std_ulong_and_error TempResult = __std_fs_get_current_path(MaxPathSize, CurrentPathDataWide);
			BS_CHECK(TempResult._Size < MaxPathSize);
			OutErrorCode = MakeErrorCode(TempResult._Error);
			char* CurrentPathData = (char*)FMemory::Allocate(MaxPathSize, EAllocationType::Temporary);
			wcstombs(OutCurrentPath, CurrentPathDataWide, TempResult._Size);
			return TempResult._Size;
		}

		NODISCARD FORCEINLINE static FString GetCurrentPath()
		{
			std::error_code ErrorCode;
			char* CurrentPathData = (char*)FMemory::Allocate(MaxPathSize, EAllocationType::Temporary);
			const size_t ResultSize = GetCurrentPath(CurrentPathData, ErrorCode);
			BS_LOG_IF(ErrorCode, Fatal, "FFilesystem::GetCurrentPath: {}", std::_Syserror_map(ErrorCode.value()));
			return FString(CurrentPathData, ResultSize);
		}

	private:
		static inline constexpr std::filesystem::_Is_slash_oper IsSlash{};
		static inline constexpr size_t MaxPathSize = __std_fs_max_path;

		NODISCARD FORCEINLINE static void CheckPathSize(FStringView InPath)
		{
			BS_ASSERT(InPath.GetSize() <= MaxPathSize,
				"The path was longer than the maximum size: Size - {}, Max - {}",
				InPath.GetSize(),
				MaxPathSize);
		}

		NODISCARD FORCEINLINE static std::error_code MakeErrorCode(__std_win_error Errno)
		{
			return {static_cast<int>(Errno), std::system_category()};
		}

		NODISCARD FORCEINLINE static bool IsDrivePrefix(const char* const First)
		{
			unsigned int Value;
			memcpy(&Value, First, sizeof(Value));
			Value &= 0xFFFF'FFDFu;
			Value -= (static_cast<unsigned int>(':') << (sizeof(char) * CHAR_BIT)) | 'A';
			return Value < 26;
		}

		NODISCARD FORCEINLINE static bool HasDriveLetterPrefix(const char* const First, const char* const Last)
		{
			return Last - First >= 2 && IsDrivePrefix(First);
		}

		NODISCARD static const char* FindRootNameEnd(const char* const First, const char* const Last)
		{
			if (Last - First < 2)
			{
				return First;
			}
			if (HasDriveLetterPrefix(First, Last))
			{
				return First + 2;
			}
			if (!IsSlash(First[0]))
			{
				return First;
			}
			if (Last - First >= 4 && IsSlash(First[3]) && (Last - First == 4 || !IsSlash(First[4]))
				&& ((IsSlash(First[1]) && (First[2] == '?' || First[2] == '.'))
				|| (First[1] == '?' && First[2] == '?')))
			{
				return First + 3;
			}
			if (Last - First >= 3 && IsSlash(First[1]) && !IsSlash(First[2]))
			{
				return std::find_if(First + 3, Last, IsSlash);
			}
			return First;
		}

		NODISCARD FORCEINLINE static const char* FindRelativePath(const char* const First, const char* const Last)
		{
			return std::find_if_not(FindRootNameEnd(First, Last), Last, IsSlash);
		}

		NODISCARD FORCEINLINE static const char* FindFilename(const char* const First, const char* Last)
		{
			const char* const RelativePath = FindRelativePath(First, Last);
			while (RelativePath != Last && !IsSlash(Last[-1]))
			{
				--Last;
			}
			return Last;
		}

		NODISCARD static constexpr const char* FindExtension(const char* const Filename, const char* const Ads)
		{
			const char* Extension = Ads;
			if (Filename == Extension)
			{
				return Ads;
			}

			--Extension;
			if (Filename == Extension)
			{
				return Ads;
			}

			if (*Extension == '.')
			{
				if (Filename == Extension - 1 && Extension[-1] == '.')
				{
					return Ads;
				}
				else
				{
					return Extension;
				}
			}

			while (Filename != --Extension)
			{
				if (*Extension == '.')
				{
					return Extension;
				}
			}

			return Ads;
		}

		NODISCARD FORCEINLINE static FStringView ParseExtension(FStringView View)
		{
			const char* const First = View.GetData();
			const char* const Last = First + View.GetSize();
			const char* const Filename = FindFilename(First, Last);
			const char* const Ads = std::find(Filename, Last, ':');
			const char* const Extension = FindExtension(Filename, Ads);
			return FStringView(Extension, Ads - Extension);
		}

		NODISCARD FORCEINLINE static FStringView ParseRootName(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			const char* const Last = First + InPath.GetSize();
			return FStringView(First, FindRootNameEnd(First, Last) - First);
		}

		NODISCARD FORCEINLINE static FStringView ParseRootDirectory(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			const char* const Last = First + InPath.GetSize();
			const char* const RootNameEnd = FindRootNameEnd(First, Last);
			const char* const RelativePath = std::find_if_not(RootNameEnd, Last, IsSlash);
			return FStringView(RootNameEnd, RelativePath - RootNameEnd);
		}

		NODISCARD FORCEINLINE static FStringView ParseRootPath(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			const char* const Last = First + InPath.GetSize();
			return FStringView(First, FindRelativePath(First, Last) - First);
		}

		NODISCARD FORCEINLINE static FStringView ParseRelativePath(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			const char* const Last = First + InPath.GetSize();
			const char* const RelativePath = FindRelativePath(First, Last);
			return FStringView(RelativePath, Last - RelativePath);
		}

		NODISCARD FORCEINLINE static FStringView ParseParentPath(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			auto Last = First + InPath.GetSize();
			const char* const RelativePath = FindRelativePath(First, Last);
			while (RelativePath != Last && !IsSlash(Last[-1]))
			{
				--Last;
			}
			while (RelativePath != Last && IsSlash(Last[-1]))
			{
				--Last;
			}
			return FStringView(First, Last - First);
		}

		NODISCARD FORCEINLINE static FStringView ParseFilename(FStringView InPath)
		{
			const char* const First = InPath.GetData();
			const char* const Last = First + InPath.GetSize();
			const char* const Filename = FindFilename(First, Last);
			return FStringView(Filename, Last - Filename);
		}
	};
}
