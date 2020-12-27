#pragma once

#include <fstream>
#include <vector>
#include <stdexcept>

enum class ShaderType_t
{
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_PIXEL
};

template<ShaderType_t RequestedType> 
class Shader
{
    ID3D10Device* m_pD3DDevice = nullptr;

    std::vector<char>* m_vecFileData;
    std::string m_ShaderName;
    
    Microsoft::WRL::ComPtr<ID3D10VertexShader> m_pVertexShaderObject;
    Microsoft::WRL::ComPtr<ID3D10PixelShader> m_pPixelShaderObject;

public:

    inline Shader(ID3D10Device* pD3DDevice, std::string Name)
        : m_pD3DDevice(pD3DDevice), m_ShaderName(Name), m_vecFileData(nullptr)
    {}

    inline Shader(ID3D10Device* pD3DDevice, std::string Name, std::vector<char>* FileData)
        : m_pD3DDevice(pD3DDevice), m_ShaderName(Name), m_vecFileData(FileData)
    {
        if (FileData == nullptr)
        {
            throw std::runtime_error{ "File data was 0" };
        }

        CreateShader();
    }

    inline Shader(ID3D10Device* pD3DDevice, std::string Name, std::string FileName)
        : m_pD3DDevice(pD3DDevice), m_ShaderName(Name)
    {
        LoadFromFile(FileName);

        CreateShader();
    }
    
    inline ~Shader()
    {}

    inline void LoadFromFile(std::string FileName)
    {
        std::ifstream VertexFile(FileName, std::ios::in | std::ios::binary | std::ios::ate);

        if (!VertexFile.is_open())
        {
            throw std::runtime_error{ "Failed to open file!" };
        }

        int Length = (int)VertexFile.tellg();

        m_vecFileData = new std::vector<char>(Length);
        VertexFile.seekg(0, std::ios::beg);
        VertexFile.read(m_vecFileData->data(), Length);
        VertexFile.close();
    }

    inline void CreateShader()
    {
        if (RequestedType == ShaderType_t::SHADER_TYPE_VERTEX)
        {
            m_pD3DDevice->CreateVertexShader(m_vecFileData->data(), m_vecFileData->size(), &m_pVertexShaderObject);

            m_pD3DDevice->VSSetShader(m_pVertexShaderObject.Get());

            return;
        }

        if (RequestedType == ShaderType_t::SHADER_TYPE_PIXEL)
        {
            m_pD3DDevice->CreatePixelShader(m_vecFileData->data(), m_vecFileData->size(), &m_pPixelShaderObject);

            m_pD3DDevice->PSSetShader(m_pPixelShaderObject.Get());

            return;
        }

        throw std::runtime_error{ "Invalid shader type given." };
    }

    inline void* GetShaderObject()
    {
        if (RequestedType == ShaderType_t::SHADER_TYPE_VERTEX)
        {
            return (void*)m_pVertexShaderObject.Get();
        }

        if (RequestedType == ShaderType_t::SHADER_TYPE_PIXEL)
        {
            return (void*)m_pPixelShaderObject.Get();
        }

        return nullptr;
    }

    inline std::vector<char>* GetFileData()
    {
        return m_vecFileData;
    }
};