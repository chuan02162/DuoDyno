//
// Created by 韩昱川 on 2/19/24.
//

#ifndef DUODYNO_RENDER_H
#define DUODYNO_RENDER_H

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <iostream>
#include <simd/simd.h>
#include <vector>

#include "view/view_settings.h"
#include "view/task_queue.h"
#include "controller/controller.h"

namespace shader_types {
	struct InstanceData {
		simd::float4x4 instanceTransform;
		simd::float4 instanceColor{};
	};
}

template<class ShapeType>
class Renderer {
public:
	explicit Renderer(MTL::Device *pDevice);

	~Renderer();

	void buildShaders();

	void buildBuffers();

	void draw(MTK::View *pView);

	void addShape(std::shared_ptr<Shape> shape);

	void updatePosition(int index, std::shared_ptr<simd::float4x4> trans);

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
	size_t _pNumCapacity;
	std::vector<std::shared_ptr<Shape>> _pShapes;
	static const int kMaxFramesInFlight = 3;
};

#pragma region ViewDelegate {

template<class ShapeType>
Renderer<ShapeType>::Renderer(MTL::Device *pDevice)
		: _pDevice(pDevice->retain()), _angle(0.f), _frame(0), _pNumCapacity(1) {
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

	auto shapeData=ShapeType::getData();

	auto vertexBufferSize=sizeof(simd::float3)*(shapeData->_pVerts->size());
	auto indicesBufferSize=sizeof(uint16_t)*(shapeData->_pIndices->size());


	MTL::Buffer *pVertexBuffer = _pDevice->newBuffer(vertexBufferSize, MTL::ResourceStorageModeManaged);
	MTL::Buffer *pIndexBuffer = _pDevice->newBuffer(indicesBufferSize, MTL::ResourceStorageModeManaged);

	_pVertexDataBuffer = pVertexBuffer;
	_pIndexBuffer = pIndexBuffer;

	memcpy(_pVertexDataBuffer->contents(), shapeData->_pVerts->data(), vertexBufferSize);
	memcpy(_pIndexBuffer->contents(), shapeData->_pIndices->data(), indicesBufferSize);


	_pVertexDataBuffer->didModifyRange(NS::Range::Make(0, _pVertexDataBuffer->length()));
	_pIndexBuffer->didModifyRange(NS::Range::Make(0, _pIndexBuffer->length()));

	const size_t instanceDataSize = _pNumCapacity * sizeof(shader_types::InstanceData);
	for (auto &i: _pInstanceDataBuffer) {
		i->release();
		i = _pDevice->newBuffer(instanceDataSize, MTL::ResourceStorageModeManaged);
	}
}

template<class ShapeType>
void Renderer<ShapeType>::draw(MTK::View *pView) {
	using simd::float4;
	using simd::float4x4;

	static auto controller = Controller::getInstance();
	controller->beforeDraw();

	static auto queueIns = TaskQueue::getInstance();
	while (!queueIns->empty()) {
		auto task = queueIns->top();
		queueIns->pop();
		switch (task->tt) {
			case TaskType::Add: {
				addShape(std::static_pointer_cast<Shape>(task->data));
				break;
			}
			case TaskType::Update: {
				updatePosition(task->index, std::static_pointer_cast<simd::float4x4>(task->data));
				break;
			}
		}
		TaskPool::getInstance()->releaseTask(task);
	}

	if (_pShapes.empty()) {
		return;
	}

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
	for (size_t i = 0; i < _pShapes.size(); ++i) {
		pInstanceData[i].instanceTransform = *(_pShapes[i]->transform);
		pInstanceData[i].instanceColor = (float4) {0, 0, 1, 1.0f};
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
								_pShapes.size());

	pEnc->endEncoding();
	pCmd->presentDrawable(pView->currentDrawable());
	pCmd->commit();

	pPool->release();
}

template<class ShapeType>
void Renderer<ShapeType>::addShape(std::shared_ptr<Shape> shape) {
	if (_pShapes.size() >= _pNumCapacity)
		buildBuffers();
	_pShapes.push_back(std::move(shape));
}

template<class ShapeType>
void Renderer<ShapeType>::updatePosition(int index, std::shared_ptr<simd::float4x4> trans) {
	if (index >= _pShapes.size()) {
		std::cout << "Wrong index at updatePosition" << std::endl;
		return;
	}
	_pShapes[index]->transform = std::move(trans);
}

#pragma endregion ViewDelegate }

#endif //DUODYNO_RENDER_H
