#include "BoxDemo.h"

BoxDemo::BoxDemo(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
: Game(instance, windowClass, windowTitle, showCommand),
	mFX(nullptr), mTech(nullptr), mfxWorldViewProj(nullptr),
	mBoxIB(nullptr), mBoxVB(nullptr)
{
	mDepthStencilBufferEnabled = true;
	mMultiSamplingEnabled = true;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
}


BoxDemo::~BoxDemo()
{
	ReleaseObject(mFX);
	ReleaseObject(mTech);
	ReleaseObject(mfxWorldViewProj);
	ReleaseObject(mBoxIB);
	ReleaseObject(mBoxVB);
}


void BoxDemo::Initialize()
{
	BuildFX();
	BuildVertexLayout();
	BuildGeometryBuffers();
}

void BoxDemo::BuildFX()
{
	HRESULT hr = S_OK;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif // defined(DEBUG) || defined(_DEBUG)

	/*ID3DBlob* compiledShader = nullptr;
	ID3DBlob* compilationMsgs = nullptr;

	HRESULT hr = D3DCompileFromFile(L"FX/color.fx", nullptr, nullptr, nullptr, "fx_5_0", shaderFlags, 0, &compiledShader, &compilationMsgs);
	if (compilationMsgs != nullptr)
	{
		GameException ex((char*)compilationMsgs->GetBufferPointer(), hr);
		ReleaseObject(compilationMsgs);	
		throw ex;
	}
	if (FAILED(hr))
	{
		ReleaseObject(compiledShader);
		throw GameException("D3DCompileFromFile() failed.");
	}*/

	std::vector<char> compiledShader;
	Utility::LoadBinaryFile(L"FX/color.cso", compiledShader);

	hr = D3DX11CreateEffectFromMemory(&compiledShader.front(), compiledShader.size(), 0, mDirect3DDevice, &mFX);
	if (FAILED(hr))
	{
		throw GameException("D3DX11CreateEffectFromMemory() failed.", hr);
	}

	mTech = mFX->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();

}

void BoxDemo::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);

	if (FAILED(mDirect3DDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &mInputLayout)))
	{
		throw GameException("CreateInputLayout() failed.");
	}

}

void BoxDemo::BuildGeometryBuffers()
{
	// Create vertex buffer
	Vertex vertices[] = 
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (XMFLOAT4)Colors::White },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), (XMFLOAT4)Colors::Black },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), (XMFLOAT4)Colors::Red },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), (XMFLOAT4)Colors::Green },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), (XMFLOAT4)Colors::Blue },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), (XMFLOAT4)Colors::Yellow },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), (XMFLOAT4)Colors::Cyan },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), (XMFLOAT4)Colors::Magenta }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)* 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0; 
	vbd.MiscFlags = 0; 
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vintData;
	vintData.pSysMem = vertices;

	if (FAILED(mDirect3DDevice->CreateBuffer(&vbd, &vintData, &mBoxVB)))
	{
		throw GameException("BuildGeometryBuffers create vertex buffer failed.");
	}

	//Create index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	if (FAILED(mDirect3DDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB)))
	{
		throw GameException("BuildGeometryBuffers create index buffer failed.");
	}

}

void BoxDemo::Update(const GameTime& gameTime)
{

}

void BoxDemo::Draw(const GameTime& gameTime)
{
	mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mDirect3DDeviceContext->IASetInputLayout(mInputLayout);
	mDirect3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mDirect3DDeviceContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	mDirect3DDeviceContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		mTech->GetPassByIndex(p)->Apply(0, mDirect3DDeviceContext);
		mDirect3DDeviceContext->DrawIndexed(36, 0, 0);
	}

	if (FAILED(mSwapChain->Present(0, 0)))
	{
		throw GameException("Draw Present failed.");
	}
}
