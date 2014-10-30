#pragma once
#include <Game.h>
#include <d3dcompiler.h>
#include <GameException.h>
#include <d3dx11effect.h>
#include <Utility.h>

using namespace Library;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class BoxDemo : public Game
{
public:
	BoxDemo(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
	~BoxDemo();

	virtual void Initialize() override;
	virtual void Update(const GameTime& gameTime) override;
	virtual void Draw(const GameTime& gameTime) override;

private:
	void BuildFX();
	void BuildVertexLayout();
	void BuildGeometryBuffers();

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};

