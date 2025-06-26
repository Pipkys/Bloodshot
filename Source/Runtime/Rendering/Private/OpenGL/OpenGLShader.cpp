#include "Shader.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	namespace Private::OpenGL::Shader
	{
		NODISCARD FORCEINLINE static GLuint CreateShader(const GLenum Type, FStringView Src)
		{
			const char* RawSrc = Src.GetData();
			const GLuint ShaderID = glCreateShader(Type);
			glShaderSource(ShaderID, 1, &RawSrc, nullptr);
			glCompileShader(ShaderID);
			GLint CompileResult;
			glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &CompileResult);
			if (!CompileResult)
			{
				GLint LogLength = 0;
				glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
				GLchar* const LogBuffer = reinterpret_cast<GLchar*>(FMemory::Allocate(LogLength, EAllocationType::Temporary));
				glGetProgramInfoLog(ShaderID, LogLength, nullptr, LogBuffer);
				BS_LOG(Fatal, "{} shader compile error: {}", OpenGLShaderTypeToString(Type), (const char*)LogBuffer);
			}
			return ShaderID;
		}
	}

	FShader::FShader(FStringView InName, FStringView InVertexShaderSrc, FStringView InFragmentShaderSrc)
	{
		const GLuint VertexShaderID = Private::OpenGL::Shader::CreateShader(GL_VERTEX_SHADER, InVertexShaderSrc);
		const GLuint FragmentShaderID = Private::OpenGL::Shader::CreateShader(GL_FRAGMENT_SHADER, InFragmentShaderSrc);

		UniqueID = glCreateProgram();
		glAttachShader(UniqueID, VertexShaderID);
		glAttachShader(UniqueID, FragmentShaderID);
		glLinkProgram(UniqueID);

		GLint LinkResult;
		glGetProgramiv(UniqueID, GL_LINK_STATUS, &LinkResult);

		if (!LinkResult)
		{
			GLint LogLength = 0;
			glGetShaderiv(UniqueID, GL_INFO_LOG_LENGTH, &LogLength);
			GLchar* const LogBuffer = reinterpret_cast<GLchar*>(FMemory::Allocate(LogLength, EAllocationType::Temporary));
			glGetProgramInfoLog(UniqueID, LogLength, nullptr, LogBuffer);
			BS_LOG(Fatal, "{} shader link error: {}", InName, (const char*)LogBuffer);
		}

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);
	}

	FShader::~FShader()
	{
		glDeleteProgram(UniqueID);
	}

	void FShader::Bind() const
	{
		glUseProgram(UniqueID);
	}

	void FShader::Unbind() const
	{
		glUseProgram(0);
	}

	void FShader::SetUniformInt(FStringView InName, const int InValue)
	{
		glProgramUniform1i(UniqueID, glGetUniformLocation(UniqueID, InName.GetData()), InValue);
	}

	void FShader::SetUniformFloat(FStringView InName, const float InValue)
	{
		glProgramUniform1f(UniqueID, glGetUniformLocation(UniqueID, InName.GetData()), InValue);
	}

	void FShader::SetUniformVec3(FStringView InName, const glm::vec3& InValue)
	{
		glProgramUniform3f(UniqueID, glGetUniformLocation(UniqueID, InName.GetData()), InValue.x, InValue.y, InValue.z);
	}

	void FShader::SetUniformVec4(FStringView InName, const glm::vec4& InValue)
	{
		glProgramUniform4f(UniqueID, glGetUniformLocation(UniqueID, InName.GetData()), InValue.x, InValue.y, InValue.z, InValue.w);
	}

	void FShader::SetUniformMat4(FStringView InName, const glm::mat4& InValue)
	{
		glProgramUniformMatrix4fv(UniqueID, glGetUniformLocation(UniqueID, InName.GetData()), 1, GL_FALSE, glm::value_ptr(InValue));
	}
}
