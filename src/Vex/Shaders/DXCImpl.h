#pragma once

#include <array>
#include <vector>

// Include dependencies required by DXC.
#if defined(_WIN32)
#include <unknwn.h>
#include <dxcapi.h>
#include <windows.h>
#include <wrl/client.h>

#if defined(__MINGW32__)
struct IDxcBlob;
struct IDxcBlobEncoding;
struct IDxcBlobUtf8;
struct IDxcIncludeHandler;
struct IDxcCompilerArgs;
struct IDxcUtils;
struct IDxcResult;
struct IDxcCompiler3;
__CRT_UUID_DECL(IDxcBlob, 0x8BA5FB08,0x5195,0x40e2,0xAC,0x58,0x0D,0x98,0x9C,0x3A,0x01,0x02)
__CRT_UUID_DECL(IDxcBlobEncoding, 0x7241d424,0x2646,0x4191,0x97,0xc0,0x98,0xe9,0x6e,0x42,0xfc,0x68)
__CRT_UUID_DECL(IDxcBlobUtf8, 0x3DA636C9,0xBA71,0x4024,0xA3,0x01,0x30,0xCB,0xF1,0x25,0x30,0x5B)
__CRT_UUID_DECL(IDxcIncludeHandler, 0x7f61fc7d,0x950d,0x467f,0xb3,0xe3,0x3c,0x02,0xfb,0x49,0x18,0x7c)
__CRT_UUID_DECL(IDxcCompilerArgs, 0x73EFFE2A,0x70DC,0x45F8,0x96,0x90,0xEF,0xF6,0x4C,0x02,0x42,0x9D)
__CRT_UUID_DECL(IDxcUtils, 0x4605C4CB,0x2019,0x492A,0xAD,0xA4,0x65,0xF2,0x0B,0xB7,0xD6,0x7F)
__CRT_UUID_DECL(IDxcResult, 0x58346CDA,0xDDE7,0x4497,0x94,0x61,0x6F,0x87,0xAF,0x5E,0x06,0x59)
__CRT_UUID_DECL(IDxcCompiler3, 0x228B4687,0x5A6A,0x4730,0x90,0x0C,0x97,0x02,0xB2,0x20,0x3F,0x54)
#endif

namespace vex
{
template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
} // namespace vex

#elif defined(__linux__)
#define __EMULATE_UUID 1
// DXC exposes an adapter for non-windows platforms.
#include <dxc/WinAdapter.h>
#include <dxc/dxcapi.h>

namespace vex
{
template <class T>
using ComPtr = CComPtr<T>;
}
#endif

#include <Vex/Shaders/CompilerBase.h>

namespace vex
{
struct ShaderKey;
struct ShaderDefine;

struct DXCCompilerImpl : public CompilerBase
{
    DXCCompilerImpl(std::vector<std::filesystem::path> includeDirectories = {});
    virtual ~DXCCompilerImpl() override;

    virtual std::expected<SHA1HashDigest, std::string> GetShaderCodeHash(
        const Shader& shader,
        const ShaderEnvironment& shaderEnv,
        const ShaderCompilerSettings& compilerSettings) override;
    virtual std::expected<ShaderCompilationResult, std::string> CompileShader(
        const Shader& shader,
        const ShaderEnvironment& shaderEnv,
        const ShaderCompilerSettings& compilerSettings) const override;

private:
    void FillInIncludeDirectories(std::vector<LPCWSTR>& args, std::vector<std::wstring>& wStrings) const;

    std::expected<ComPtr<IDxcResult>, std::string> CompileShader(
        const ShaderKey& key,
        const std::vector<std::wstring>& args,
        const std::vector<std::pair<std::wstring, std::wstring>>& defines,
        const DxcBuffer& shaderSource) const;

    ComPtr<IDxcCompiler3> compiler;
    ComPtr<IDxcUtils> utils;
    ComPtr<IDxcIncludeHandler> defaultIncludeHandler;
};

} // namespace vex