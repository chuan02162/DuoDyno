//
// Created by 韩昱川 on 2/19/24.
//

#ifndef DUODYNO_RENDER_H
#define DUODYNO_RENDER_H

#endif //DUODYNO_RENDER_H


#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <iostream>
#include <simd/simd.h>
#include <vector>
#include "view/view_settings.h"
#include "view/common/shape.h"
#include "shape_factory.h"

namespace shader_types {
	struct InstanceData {
		simd::float4x4 instanceTransform;
		[[maybe_unused]] simd::float4 instanceColor{};
	};
}

template<class ShapeType>
class Renderer {
public:
	Renderer(MTL::Device *pDevice);

	~Renderer();

	void buildShaders();

	void buildBuffers();

	void draw(MTK::View *pView);

	void addShape(const ShapeType *shape);

	void resizeBuffer(size_t newSize);

private:
	MTL::Device *_pDevice;
	MTL::CommandQueue *_pCommandQueue;
	MTL::Library *_pShaderLibrary{};
	MTL::RenderPipelineState *_pPSO{};
	MTL::Buffer *_pVertexDataBuffer{};
	MTL::Buffer *_pInstanceDataBuffer[view_settings::kMaxFramesInFlight]{};
	MTL::Buffer *_pIndexBuffer{};
	float _angle;
	dispatch_semaphore_t _semaphore;
	int _frame;
	size_t _pNumInstances = 3;
	size_t _pNumCapacity;
	ShapeType *_pShapes;
	static const int kMaxFramesInFlight=3;
};

static constexpr size_t kNumInstances = 1;
static constexpr size_t kMaxFramesInFlight = 3;

//class Renderer
//{
//public:
//	Renderer( MTL::Device* pDevice );
//	~Renderer();
//	void buildShaders();
//	void buildBuffers();
//	void draw( MTK::View* pView );
//
//private:
//	MTL::Device* _pDevice;
//	MTL::CommandQueue* _pCommandQueue;
//	MTL::Library* _pShaderLibrary;
//	MTL::RenderPipelineState* _pPSO;
//	MTL::Buffer* _pVertexDataBuffer;
//	MTL::Buffer* _pInstanceDataBuffer[kMaxFramesInFlight];
//	MTL::Buffer* _pIndexBuffer;
//	float _angle;
//	int _frame;
//	dispatch_semaphore_t _semaphore;
//	static const int kMaxFramesInFlight;
//};


#pragma region ViewDelegate {

template<class ShapeType>
Renderer<ShapeType>::Renderer(MTL::Device *pDevice)
		: _pDevice(pDevice->retain()), _angle(0.f), _frame(0), _pNumInstances(0), _pNumCapacity(16) {
	_pCommandQueue = _pDevice->newCommandQueue();
	buildShaders();
	buildBuffers();
	_semaphore = dispatch_semaphore_create(view_settings::kMaxFramesInFlight);
}

template<class ShapeType>
Renderer<ShapeType>::~Renderer() {
	_pShaderLibrary->release();
	_pVertexDataBuffer->release();
	for (auto &i: _pInstanceDataBuffer) {
		i->release();
	}
	_pIndexBuffer->release();
	_pPSO->release();
	_pCommandQueue->release();
	_pDevice->release();
}

template<class ShapeType>
void Renderer<ShapeType>::buildShaders() {
	using NS::StringEncoding::UTF8StringEncoding;

	const char *shaderSrc = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct v2f
        {
            float4 position [[position]];
            half3 color;
        };

        struct VertexData
        {
            float3 position;
        };

        struct InstanceData
        {
            float4x4 instanceTransform;
            float4 instanceColor;
        };

        v2f vertex vertexMain( device const VertexData* vertexData [[buffer(0)]],
                               device const InstanceData* instanceData [[buffer(1)]],
                               uint vertexId [[vertex_id]],
                               uint instanceId [[instance_id]] )
        {
            v2f o;
            float4 pos = float4( vertexData[ vertexId ].position, 1.0 );
            o.position = instanceData[ instanceId ].instanceTransform * pos;
            o.color = half3( instanceData[ instanceId ].instanceColor.rgb );
            return o;
        }

        half4 fragment fragmentMain( v2f in [[stage_in]] )
        {
            return half4( in.color, 1.0 );
        }
    )";

	NS::Error *pError = nullptr;
	MTL::Library *pLibrary = _pDevice->newLibrary(NS::String::string(shaderSrc, UTF8StringEncoding), nullptr, &pError);
	if (!pLibrary) {
		__builtin_printf("%s", pError->localizedDescription()->utf8String());
		assert(false);
	}

	MTL::Function *pVertexFn = pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
	MTL::Function *pFragFn = pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

	MTL::RenderPipelineDescriptor *pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
	pDesc->setVertexFunction(pVertexFn);
	pDesc->setFragmentFunction(pFragFn);
	pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

	_pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
	if (!_pPSO) {
		__builtin_printf("%s", pError->localizedDescription()->utf8String());
		assert(false);
	}

	pVertexFn->release();
	pFragFn->release();
	pDesc->release();
	_pShaderLibrary = pLibrary;
}

template<class ShapeType>
void Renderer<ShapeType>::buildBuffers() {
	using simd::float4;
	using simd::float4x4;
	_pShapes = new Shape[_pNumCapacity];
	auto sf = ShapeFactory::getInstance();
	auto sq = sf.createShape("Square");
	sq->Transform = new float4x4{(float4) {0.001, 0.1, 0.f, 0.f},
								 (float4) {0.1, -0.001, 0.f, 0.f},
								 (float4) {0.f, 0.f, 0.1, 0.f},
								 (float4) {0.f, 0.06f, 0.f, 1.f}};
	static int flag = 0;
	if (!flag)
		addShape(sq);
	flag = 1;

	MTL::Buffer *pVertexBuffer = _pDevice->newBuffer(ShapeType::getVertsSize(), MTL::ResourceStorageModeManaged);
	MTL::Buffer *pIndexBuffer = _pDevice->newBuffer(ShapeType::getIndicesSize(), MTL::ResourceStorageModeManaged);

	_pVertexDataBuffer = pVertexBuffer;
	_pIndexBuffer = pIndexBuffer;

	ShapeType::writeVertsData(_pVertexDataBuffer->contents());
	ShapeType::writeIndicesData(_pIndexBuffer->contents());

	_pVertexDataBuffer->didModifyRange(NS::Range::Make(0, _pVertexDataBuffer->length()));
	_pIndexBuffer->didModifyRange(NS::Range::Make(0, _pIndexBuffer->length()));

	const size_t instanceDataSize = _pNumCapacity * sizeof(shader_types::InstanceData);
	for (auto &i: _pInstanceDataBuffer) {
		i->release();
		i = _pDevice->newBuffer(1, MTL::ResourceStorageModeManaged);
	}
}

template<class ShapeType>
void Renderer<ShapeType>::draw(MTK::View *pView) {
	using simd::float4;
	using simd::float4x4;

	NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();

	_frame = (_frame + 1) % int(view_settings::kMaxFramesInFlight);
	MTL::Buffer *pInstanceDataBuffer = _pInstanceDataBuffer[_frame];

	MTL::CommandBuffer *pCmd = _pCommandQueue->commandBuffer();
	dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
	pCmd->addCompletedHandler(^void(MTL::CommandBuffer *pCmd) {
		dispatch_semaphore_signal(this->_semaphore);
	});

	_angle += 0.01f;


	auto *pInstanceData = reinterpret_cast< shader_types::InstanceData *>( pInstanceDataBuffer->contents());
	const float scl = 0.1f;
	for (size_t i = 0; i < _pNumInstances; ++i) {
		float iDivNumInstances = i / (float)_pNumInstances;
		float xoff = (iDivNumInstances * 2.0f - 1.0f) + (1.f/_pNumInstances);
		float yoff = sin( ( iDivNumInstances + _angle ) * 2.0f * M_PI);
		std::cout<<xoff<<" "<<yoff<<std::endl;
		pInstanceData[i].instanceTransform = *(_pShapes[i].Transform);
//		pInstanceData[ i ].instanceTransform = (float4x4){ (float4){ scl * sinf(_angle), scl * cosf(_angle), 0.f, 0.f },
//														   (float4){ scl * cosf(_angle), scl * -sinf(_angle), 0.f, 0.f },
//														   (float4){ 0.f, 0.f, scl, 0.f },
//														   (float4){ xoff, yoff, 0.f, 1.f } };
//		float r = iDivNumInstances;
//		float g = 1.0f - r;
//		float b = sinf( M_PI * 2.0f * iDivNumInstances );
		pInstanceData[ i ].instanceColor = (float4){ 0, 1, 0, 1.0f };
	}
	pInstanceDataBuffer->didModifyRange(NS::Range::Make(0, pInstanceDataBuffer->length()));

	MTL::RenderPassDescriptor *pRpd = pView->currentRenderPassDescriptor();
	MTL::RenderCommandEncoder *pEnc = pCmd->renderCommandEncoder(pRpd);

	pEnc->setRenderPipelineState(_pPSO);
	pEnc->setVertexBuffer(_pVertexDataBuffer, 0, 0);
	pEnc->setVertexBuffer(pInstanceDataBuffer, 0, 1);

	pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
								6, MTL::IndexType::IndexTypeUInt16,
								_pIndexBuffer,
								0,
								_pNumInstances);

	pEnc->endEncoding();
	pCmd->presentDrawable(pView->currentDrawable());
	pCmd->commit();

	pPool->release();
}

template<class ShapeType>
void Renderer<ShapeType>::resizeBuffer(size_t newSize) {
	if (newSize > view_settings::kMaxInstances) {
		printf("Exceed instance number limit, failed to resize!");
		return;
	}
	while (_pNumCapacity < newSize) {
		_pNumCapacity <<= 1;
	}
	_pNumCapacity = std::min(_pNumCapacity, view_settings::kMaxInstances);
	buildBuffers();
}

template<class ShapeType>
void Renderer<ShapeType>::addShape(const ShapeType *shape) {
	if (_pNumInstances > _pNumCapacity)
		resizeBuffer(_pNumInstances);
	_pShapes[_pNumInstances] = *shape;
	++_pNumInstances;
}

#pragma endregion ViewDelegate }

//