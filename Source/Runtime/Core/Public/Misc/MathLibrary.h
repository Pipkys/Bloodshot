#pragma once

#pragma warning(push)
#pragma warning(disable: 26495) // Variable '(ForwardVector, RightVector, UpVector)' is uninitialized

#include "Platform/Platform.h"

THIRD_PARTY_INCLUDES_START
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
THIRD_PARTY_INCLUDES_END

#include <random>
#include <cmath>

namespace Bloodshot
{
	template<typename T>
	class TRandom final
	{
	public:
		NODISCARD FORCEINLINE static T Get()
		{
			return s_UniformDistribution(s_Engine);
		}

		NODISCARD FORCEINLINE static T Get(const T Min, const T Max)
		{
			return std::uniform_int_distribution<T>(Min, Max)(s_Engine);
		}

	private:
		static inline std::random_device s_RandomDevice;
		static inline std::mt19937_64 s_Engine{s_RandomDevice()};
		static inline std::uniform_int_distribution<T> s_UniformDistribution;
	};

	struct IVector3Constants final
	{
		static constexpr inline glm::vec3 Up = glm::vec3(0.f, 0.f, 1.f);
		static constexpr inline glm::vec3 Right = glm::vec3(0.f, 1.f, 0.f);
		static constexpr inline glm::vec3 Forward = glm::vec3(1.f, 0.f, 0.f);
	};

	class FTransform
	{
	public:
		NODISCARD FORCEINLINE bool operator==(const FTransform& Other)
		{
			return MyPosition == Other.MyPosition
				&& MyRotation == Other.MyRotation
				&& MyScale == Other.MyScale;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetPosition() const
		{
			return MyPosition;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetRotation() const
		{
			return MyRotation;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetScale() const
		{
			return MyScale;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetForwardVector() const
		{
			if (bDirty)
			{
				UpdateVectors();
			}
			return ForwardVector;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetRightVector() const
		{
			if (bDirty)
			{
				UpdateVectors();
			}
			return RightVector;
		}

		NODISCARD FORCEINLINE const glm::vec3& GetUpVector() const
		{
			if (bDirty)
			{
				UpdateVectors();
			}

			return UpVector;
		}

		FORCEINLINE void SetPosition(const glm::vec3& NewPosition)
		{
			MyPosition = NewPosition;
		}

		FORCEINLINE void SetRotation(const glm::vec3& NewRotation)
		{
			MyRotation = NewRotation;
			bDirty = true;
		}

		FORCEINLINE void SetScale(const glm::vec3& NewScale)
		{
			MyScale = NewScale;
		}

		FORCEINLINE void Translate(const glm::vec3& Translation)
		{
			MyPosition += Translation;
		}

		FORCEINLINE void Rotate(const glm::vec3& InRotation)
		{
			if (InRotation != glm::vec3(0.f, 0.f, 0.f))
			{
				MyRotation += InRotation;
				bDirty = true;
			}
		}

		FORCEINLINE void Scale(const glm::vec3& InScale)
		{
			MyScale += InScale;
		}

		FORCEINLINE void UpdateVectors() const
		{
			ForwardVector = glm::normalize(glm::quat(glm::radians(MyRotation)) * IVector3Constants::Forward);
			RightVector = glm::normalize(glm::cross(IVector3Constants::Up, ForwardVector));
			UpVector = glm::normalize(glm::cross(RightVector, ForwardVector));
			bDirty = false;
		}

		NODISCARD FORCEINLINE glm::mat4 CalculateModelMatrix() const
		{
			constexpr glm::mat4 IdentityMatrix(1.f);
			return glm::translate(IdentityMatrix, MyPosition)
				* glm::rotate(IdentityMatrix, glm::radians(MyRotation.x), IVector3Constants::Forward)
				* glm::rotate(IdentityMatrix, glm::radians(MyRotation.y), IVector3Constants::Right)
				* glm::rotate(IdentityMatrix, glm::radians(MyRotation.z), IVector3Constants::Up)
				* glm::scale(IdentityMatrix, MyScale);
		}

	private:
		glm::vec3 MyPosition = glm::vec3(0.f);
		glm::vec3 MyRotation = glm::vec3(0.f);
		glm::vec3 MyScale = glm::vec3(1.f);

		mutable glm::vec3 ForwardVector;
		mutable glm::vec3 RightVector;
		mutable glm::vec3 UpVector;

		mutable bool bDirty = true;
	};
}

#pragma warning(pop)
